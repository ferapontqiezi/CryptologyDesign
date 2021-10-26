#pragma GCC optimize("O3")
#include<stdio.h>
#include<string.h>
#include<algorithm>
#include<map>
using namespace std;

#define KEYBYTES (16)  // 密钥字节长度
#define INPUTBYTES (1<<24) // 明文字节长度
#define SPNBYTES (8)  //SPN加密长度，需要改动

// mode == 1: encrypt; mode == 0: decrypt
#define ui unsigned long long

// S表
ui substitution[2][16] = { {14,3,4,8,1,12,10,15,7,13,9,6,11,2,0,5},
						{14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7} };

// P表
int permutation[2][64] = { 
	{0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,1,5,9,13,17,21,25,29,33,37,41,45,49,53,57,61,2,6,10,14,18,22,26,30,34,38,42,46,50,54,58,62,3,7,11,15,19,23,27,31,35,39,43,47,51,55,59,63},
	{0,16,32,48,1,17,33,49,2,18,34,50,3,19,35,51,4,20,36,52,5,21,37,53,6,22,38,54,7,23,39,55,8,24,40,56,9,25,41,57,10,26,42,58,11,27,43,59,12,28,44,60,13,29,45,61,14,30,46,62,15,31,47,63}
};

// S盒代换
inline ui S_box(ui mode, ui raw) {
	return (
		  substitution[mode][(raw & 0x000000000000000f)] |
		((substitution[mode][(raw & 0x00000000000000f0) >> 4]) << 4) |
		((substitution[mode][(raw & 0x0000000000000f00) >> 8]) << 8) |
		((substitution[mode][(raw & 0x000000000000f000) >> 12]) << 12) |
		((substitution[mode][(raw & 0x00000000000f0000) >> 16]) << 16) |
		((substitution[mode][(raw & 0x0000000000f00000) >> 20]) << 20) |
		((substitution[mode][(raw & 0x000000000f000000) >> 24]) << 24) |
		((substitution[mode][(raw & 0x00000000f0000000) >> 28]) << 28) |
		((substitution[mode][(raw & 0x0000000f00000000) >> 32]) << 32) |
		((substitution[mode][(raw & 0x000000f000000000) >> 36]) << 36) |
		((substitution[mode][(raw & 0x00000f0000000000) >> 40]) << 40) |
		((substitution[mode][(raw & 0x0000f00000000000) >> 44]) << 44) |
		((substitution[mode][(raw & 0x000f000000000000) >> 48]) << 48) |
		((substitution[mode][(raw & 0x00f0000000000000) >> 52]) << 52) |
		((substitution[mode][(raw & 0x0f00000000000000) >> 56]) << 56) |
		((substitution[mode][(raw & 0xf000000000000000) >> 60]) << 60)
		);
}

// P盒代换
inline ui P_box(ui mode, ui raw) {
	ui before[64];
	ui tmp = 1;
	for (int i = 0; i < 64; ++i) {
		before[i] = ((raw & tmp) == 0 ? 0 : 1);
		tmp <<= 1;
	}
	ui mature = 0;
	for (int i = 0; i < 64; ++i) {
		mature = mature | (before[permutation[mode][i]] << i);
	}
	return mature;
}

inline ui SPN_encrypt(ui k[5], ui raw) {
	for (ui i = 0; i < 3; ++i) {
		raw ^= k[i];
		raw = S_box(1, raw);
		raw = P_box(1, raw);
	}
	raw ^= k[3];
	raw = S_box(1, raw);
	raw ^= k[4];
	return raw;
}

int main() {
	// 密钥, 明文, 密文, 初始向量IV
	ui k[5] = { 0 }, raw[1] = { 0 }, mature[1] = { 0 }, IV = 20210922;
	//freopen("E:\\CryptologyDesgin\\x64\\Debug\\1.in", "r", stdin);
	//freopen("E:\\CryptologyDesgin\\x64\\Debug\\1out.out", "w", stdout);
	fread(k, SPNBYTES, 1, stdin); // 读入k[0]
	fread(k + 4, SPNBYTES, 1, stdin); // 读入k[4]
	// 预处理PREPROCESS
	k[1] = ((k[0] << 16) & 0xffffffffffff0000) | ((k[4] >> 48) & 0x000000000000ffff);
	k[2] = ((k[0] << 32) & 0xffffffff00000000) | ((k[4] >> 32) & 0x00000000ffffffff);
	k[3] = ((k[0] << 48) & 0xffff000000000000) | ((k[4] >> 16) & 0x0000ffffffffffff);

	// CBC_Mode
	fread(raw, SPNBYTES, 1, stdin);
	raw[0] ^= IV;
	mature[0] = SPN_encrypt(k, raw[0]);
	fwrite(mature, SPNBYTES, 1, stdout);
	// CBC模式
	for (ui i = 1; i < (INPUTBYTES / SPNBYTES); ++ i) {
		fread(raw, SPNBYTES, 1, stdin); // 读入明文
		raw[0] ^= mature[0];
		mature[0] = SPN_encrypt(k, raw[0]); // SPN
		fwrite(mature, SPNBYTES, 1, stdout); // 读入密文
	}
	return 0;
}
