#include<iostream>
#include<stdio.h>

using namespace std;

// uiӲ��
#define ui unsigned int


// ����3.1��P59����S�ж������£�S[0]��ʾ���ܣ�S[1]��ʾ���ܣ���
ui substitution[2][16] = { {14, 3, 4, 8, 1, 12, 10, 15, 7, 13, 9, 6, 11, 2, 0, 5},
						   {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7} };


// S�д���
int S_box(ui mode, ui raw) {
	return (
		(substitution[mode][raw & 0x000f]) |
		((substitution[mode][(raw & 0x00f0) >> 4]) << 4) |
		((substitution[mode][(raw & 0x0f00) >> 8]) << 8) |
		((substitution[mode][(raw & 0xf000) >> 12]) << 12)
		);
}

// P���û�
int P_box(ui mode, ui raw) {
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

// SPN����(ģʽ1����Կ���������ı�)
inline int SPN_encrypt(ui key, ui raw) {

	// ǰ3��
	for (int i = 1; i <= 3; ++ i) {
		// ����Կ���w -> u��
		raw ^= ((key >> (16 - 4 * (i - 1))) & 0xffff);

		// ������u -> v��
		raw = S_box(1, raw);

		// �û���v -> w��
		raw = P_box(1, raw);
	}

	// ���1�֣����w -> u��
	raw ^= (key >> 4) & 0xffff;

	// ���1�֣�������u -> v��
	raw = S_box(1, raw);

	// ���1�֣����v -> y��
	raw ^= (key & 0xffff);

	// ���ش�
	return raw;
}

// SPN����(ģʽ0����Կ���������ı�)
inline int SPN_decrypt(ui key, ui raw) {

	// ǰ3��
	for (int i = 1; i <= 3; ++i) {
		// ����Կ���w -> u��
		if (i == 1) {
			raw ^= (key & 0xffff);
		}
		// ����ʱ����u��permutation�ˣ����Ե�2��3��4����ԿK��Ҫ��permutation������w�Ŷ�
		else {
			raw ^= P_box(0, key >> ((i - 1) << 2) & 0xffff);
		}

		// ������u -> v��
		raw = S_box(0, raw);

		// �û���v -> w��
		raw = P_box(0, raw);
	}

	// ���1�֣����w -> u��
	raw ^= P_box(0, (key >> 12) & 0xffff);

	// ���1�֣�������u -> v��
	raw = S_box(0, raw);

	// ���1�֣����v -> y��
	raw ^= ((key >> 16) & 0xffff);

	// ���ش�
	return raw;
}


// �ļ���д˲ɱ��
char buf[1 << 22], * p1 = buf, * p2 = buf;
inline int getc() {
	return p1 == p2 && (p2 = (p1 = buf) + fread(buf, 1, 1 << 22, stdin), p1 == p2) ? EOF : *p1++;
}
inline void read(ui& res){
	res = 0;
	char c = getc();
	while (c != ' ' && c != '\n') {
		res = (res << 4) + ((c >= 'a') ? (c - 'a' + 10) : (c - '0'));
		c = getc();
	}
}
char pbuf[1 << 22], * pp = pbuf;
inline void push(const char& c) {
	if (pp - pbuf == 1 << 22) fwrite(pbuf, 1, 1 << 22, stdout), pp = pbuf;
	*pp++ = c;
}
inline void write(ui ans) {
	for (int i = 1; i <= 4; ++i) {
		ui s = (ans >> (16 - 4 * i)) & 0xf;
		if (s <= 9) {
			push(s + '0');
		}
		else {
			push(s + 'a' - 10);
		}
	}
}

int main() {
	ui n, K, raw;
	ui answer1 = 0, answer2 = 0;

	scanf("%d", &n);
	getchar();
	for (int i = 1; i <= n; ++i) {
		read(K);
		read(raw);
		// ����
		answer1 = SPN_encrypt(K, raw);
		write(answer1);
		push(' ');
		// ����
		answer1 ^= 0x0001;
		answer2 = SPN_decrypt(K, answer1);
		write(answer2);
		push('\n');
	}
	fwrite(pbuf, 1, pp-pbuf, stdout);
	return 0;
}