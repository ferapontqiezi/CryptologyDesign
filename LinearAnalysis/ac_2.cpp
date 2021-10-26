#pragma GCC optimize("O3")
#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<algorithm>

using namespace std;

// ui硬干
#define ui int

// 例题3.1（P59）的S盒定义如下（S[0]表示解密，S[1]表示加密）：
ui substitution[2][16] = { {14, 3, 4, 8, 1, 12, 10, 15, 7, 13, 9, 6, 11, 2, 0, 5},
						   {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7} };

// S盒代换
ui S_box(ui mode, ui raw) {
	return (
		(substitution[mode][raw & 0x000f]) |
		((substitution[mode][(raw & 0x00f0) >> 4]) << 4) |
		((substitution[mode][(raw & 0x0f00) >> 8]) << 8) |
		((substitution[mode][(raw & 0xf000) >> 12]) << 12)
		);
}

// P盒置换
ui P_box(ui mode, ui raw) {
	ui mature;
	if (mode == 1) {
		mature = (raw & 0x0001) |
			((raw & 0x0010) >> 3) |
			((raw & 0x0100) >> 6) |
			((raw & 0x1000) >> 9) |
			((raw & 0x0002) << 3) |
			(raw & 0x0020) |
			((raw & 0x0200) >> 3) |
			((raw & 0x2000) >> 6) |
			((raw & 0x0004) << 6) |
			((raw & 0x0040) << 3) |
			(raw & 0x0400) |
			((raw & 0x4000) >> 3) |
			((raw & 0x0008) << 9) |
			((raw & 0x0080) << 6) |
			((raw & 0x0800) << 3) |
			(raw & 0x8000);
	}
	else {
		mature = (raw & 0x8000) |
			((raw & 0x0800) << 3) |
			((raw & 0x0080) << 6) |
			((raw & 0x0008) << 9) |
			((raw & 0x4000) >> 3) |
			(raw & 0x0400) |
			((raw & 0x0040) << 3) |
			((raw & 0x0004) << 6) |
			((raw & 0x2000) >> 6) |
			((raw & 0x0200) >> 3) |
			((raw & 0x0020)) |
			((raw & 0x0002) << 3) |
			((raw & 0x1000) >> 9) |
			((raw & 0x0100) >> 6) |
			((raw & 0x0010) >> 3) |
			(raw & 0x1);
	}
	return mature;
}

// SPN加密(模式1，密钥，待处理文本)
inline ui SPN_encrypt(ui key, ui raw, ui mature) {

	// 前3轮
	for (ui i = 1; i <= 3; ++i) {
		// 与密钥异或（w -> u）
		raw ^= ((key >> (16 - 4 * (i - 1))) & 0xffff);

		// 代换（u -> v）
		raw = S_box(1, raw);

		// 置换（v -> w）
		raw = P_box(1, raw);
	}

	// 最后1轮：异或（w -> u）
	raw ^= (key >> 4) & 0xffff;

	// 最后1轮：代换（u -> v）
	raw = S_box(1, raw);

	// 最后1轮：异或（v -> y）
	raw ^= (key & 0xffff);

	// 返回答案
	return (raw == mature);
}

#define getc() (_b1==_b2?fread(_b,1,1 << 21,stdin),_b2=_b+(1 << 21),*((_b1=_b)++):*(_b1++))
char _b[1 << 21], * _b1, * _b2;
inline ui read() {
	ui res = 0;
	char c = getc();
	while (c != ' ' && c != '\n') {
		res = (res << 4) + ((c >= 'a') ? (c - 'a' + 10) : (c - '0'));
		c = getc();
	}
	return res;
}

inline void write(ui ans) {
	for (int i = 1; i <= 8; ++i) {
		ui s = (ans >> (32 - 4 * i)) & 0xf;
		if (s <= 9) {
			putchar(s + '0');
		}
		else {
			putchar(s + 'a' - 10);
		}
	}
}


int main() {
	ui n, plain[8000], cipher[8000], ans;
	ui last2thkey, k[8], u[4], cnt, maxkey[2];

	// (0, 0) -> (f, f)计数器
	pair<ui, ui> cnt1[256], cnt2[256];

	//freopen("E:\\CryptologyDesgin\\LinearAnalysis\\8.in", "r", stdin);
	//freopen("E:\\CryptologyDesgin\\LinearAnalysis\\8.out", "w", stdout);

	scanf("%d", &n);
	getchar();

	for (ui i = 0; i < n; ++i) {
		last2thkey = -1;
		fill(cnt1, cnt1 + 256, pair<ui, ui>(-4000, 0));
		for (ui j = 0; j < 8000; ++j) {
			plain[j] = read();
			cipher[j] = read();
		}
		// 第一组处理u和z
		for (ui j = 0; j < 8000; ++j) {
			for (k[5] = 0; k[5] < 16; ++k[5]) {
				for (k[7] = 0; k[7] < 16; ++k[7]) {
					u[1] = substitution[0][((cipher[j] & 0x0f00) >> 8) ^ k[5]];
					u[3] = substitution[0][(cipher[j] & 0x000f) ^ k[7]];
					ui z = ((plain[j] & 0x0800) >> 11) ^ ((plain[j] & 0x0200) >> 9) ^ ((plain[j] & 0x0100) >> 8)
						^ ((u[1] & 0x4) >> 2) ^ (u[1] & 0x1) ^ ((u[3] & 0x4) >> 2) ^ (u[3] & 0x1);
					if (z == 0) {
						++cnt1[k[5] * 16 + k[7]].first;
					}
				}
			}
		}
		// 第一组结果
		for (ui t = 0; t < 256; ++t) {
			cnt1[t].second = t;
			cnt1[t].first = abs(cnt1[t].first);
		}
		sort(cnt1, cnt1 + 256);
		for (ui t = 255; t >= 0; --t) {
			last2thkey = k[5];
			k[5] = cnt1[t].second / 16;
			k[7] = cnt1[t].second % 16;
			// 第二组处理u和z
			if (t == 255 || last2thkey != k[5]) {
				fill(cnt2, cnt2 + 256, pair<ui, ui>(-4000, 0));
				for (ui j = 0; j < 8000; ++j) {
					for (k[4] = 0; k[4] < 16; ++k[4]) {
						for (k[6] = 0; k[6] < 16; ++k[6]) {
							u[0] = substitution[0][((cipher[j] & 0xf000) >> 12) ^ k[4]];
							u[1] = substitution[0][((cipher[j] & 0x0f00) >> 8) ^ k[5]];
							u[2] = substitution[0][((cipher[j] & 0x00f0) >> 4) ^ k[6]];
							ui z = ((plain[j] & 0x0800) >> 11) ^ ((plain[j] & 0x0400) >> 10) ^ ((u[0] & 0x4) >> 2)
								^ (u[0] & 0x1) ^ ((u[1] & 0x4) >> 2) ^ (u[1] & 0x1) ^ ((u[2] & 0x4) >> 2) ^ (u[2] & 0x1);
							if (z == 0) {
								cnt2[k[4] * 16 + k[6]].first++;
							}
						}
					}
				}
				// 第二组结果
				for (ui t = 0; t < 256; ++t) {
					cnt2[t].second = t;
					cnt2[t].first = abs(cnt2[t].first);
				}
				maxkey[0] = max_element(cnt2, cnt2 + 256) - cnt2;
				cnt2[maxkey[0]].first = 0;
				maxkey[1] = (*max_element(cnt2, cnt2 + 256)).second;
			}
			for (ui r = 0; r <= 1; ++r) {
				k[4] = maxkey[r] / 16;
				k[6] = maxkey[r] % 16;
				for (k[0] = 0; k[0] < 65536; ++k[0]) {
					ans = (k[0] << 16) | (k[4] << 12) | (k[5] << 8) | (k[6] << 4) | k[7];
					for (cnt = 0; cnt < 2; cnt++) {
						if (!SPN_encrypt(ans, plain[cnt], cipher[cnt]))
							break;
					}
					if (cnt == 2) {
						write(ans);
						putchar('\n');
						break;
					}
				}
				if (cnt == 2) break;
			}
			if (cnt == 2) break;
		}
	}
	return 0;
}
