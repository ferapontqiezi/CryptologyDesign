#pragma GCC optimize("O3")
#include<stdio.h>
#include<stdlib.h>
#include<algorithm>
#include<map>

using namespace std;

#define ui unsigned int

ui substitution[2][16] = { {14, 3, 4, 8, 1, 12, 10, 15, 7, 13, 9, 6, 11, 2, 0, 5},
						   {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7} };

ui S_box(ui mode, ui raw) {
	return (
		(substitution[mode][raw & 0x000f]) |
		((substitution[mode][(raw & 0x00f0) >> 4]) << 4) |
		((substitution[mode][(raw & 0x0f00) >> 8]) << 8) |
		((substitution[mode][(raw & 0xf000) >> 12]) << 12)
		);
}

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

// 是否SPN加密(密钥，明文，密文)
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
	ui n, cipher[65536], ans = 0;
	ui k[8], cnt;
	ui u1[4], u2[4];
	pair<ui, ui> cnt1[256], cnt2[256];

	//freopen("E:\\CryptologyDesgin\\x64\\Debug\\2.in", "r", stdin);

	scanf("%d", &n);
	getc();

	for (ui i = 0; i < n; ++i) {
		for (ui j = 0; j < 65536; ++j) {
			cipher[j] = read();
		}

		fill(cnt1, cnt1 + 256, pair<ui, ui>(0, 0));
		fill(cnt2, cnt2 + 256, pair<ui, ui>(0, 0));
		for (ui j = 0; j < 65536; j += 37) {
			if (((cipher[j] ^ cipher[j ^ 0x0b00]) & 0xf0f0) == 0) {
				for (k[5] = 0; k[5] < 16; ++k[5]) {
					for (k[7] = 0; k[7] < 16; ++k[7]) {
						u1[1] = substitution[0][((cipher[j] & 0x0f00) >> 8) ^ k[5]];
						u1[3] = substitution[0][((cipher[j] & 0x000f) ^ k[7])];
						u2[1] = substitution[0][((cipher[j ^ 0x0b00] & 0x0f00) >> 8) ^ k[5]];
						u2[3] = substitution[0][((cipher[j ^ 0x0b00] & 0x000f) ^ k[7])];
						if ((u1[1] ^ u2[1]) == 6 && (u1[3] ^ u2[3]) == 6)
							cnt1[k[5] * 16 + k[7]].first++;
					}
				}
			}
			if (((cipher[j] ^ cipher[j ^ 0x0050]) & 0x0f0f) == 0) {
				for (k[4] = 0; k[4] < 16; ++k[4]) {
					for (k[6] = 0; k[6] < 16; ++k[6]) {
						u1[0] = substitution[0][((cipher[j] & 0xf000) >> 12) ^ k[4]];
						u1[2] = substitution[0][((cipher[j] & 0x00f0) >> 4) ^ k[6]];
						u2[0] = substitution[0][((cipher[j ^ 0x0050] & 0xf000) >> 12) ^ k[4]];
						u2[2] = substitution[0][((cipher[j ^ 0x0050] & 0x00f0) >> 4) ^ k[6]];
						if ((u1[0] ^ u2[0]) == 5 && (u1[2] ^ u2[2]) == 5)
							cnt2[k[4] * 16 + k[6]].first++;
					}
				}
			}
		}

		for (ui t = 0; t < 256; ++t) {
			cnt1[t].second = t;
			cnt2[t].second = t;
		}

		sort(cnt1, cnt1 + 256);
		sort(cnt2, cnt2 + 256);

		k[5] = cnt1[255].second / 16;
		k[7] = cnt1[255].second % 16;
		
		for (ui t = 255; t > 0; --t) {
			k[4] = cnt2[t].second / 16;
			k[6] = cnt2[t].second % 16;

			for (k[0] = 65536; k[0] >= 0; --k[0]) {
				ans = k[0] << 16 | k[4] << 12 | k[5] << 8 | k[6] << 4 | k[7];
				for (cnt = 0; cnt < 6; ++cnt) {
					if (!SPN_encrypt(ans, 64 << cnt, cipher[64 << cnt]))
						break;
				}
				if (cnt == 6) break;
			}
			if (cnt == 6) break;
		}
		if (cnt == 6) {
			write(ans);
			putchar('\n');
		}
	}
	return 0;
}