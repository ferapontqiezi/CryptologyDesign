// 对称加密算法——SPN
#include<iostream>
#include<algorithm>
#include<string>
#include<stdio.h>
#include<stdlib.h>

using namespace std;

// unsigned short只占俩字节，为了节省内存，全篇都用unsigned short
#define us unsigned short


// 密钥key的各个bit 
struct bit4 {
	// key1: 4  表示  key1该变量是位数bit为4
	us key4 : 4, key3 : 4, key2 : 4, key1 : 4;
};

struct bit16 {
	// key1: 1  表示  key1该变量是位数bit为1
	us key16 : 1, key15 : 1, key14 : 1, key13 : 1,
		key12 : 1, key11 : 1, key10 : 1, key9 : 1,
		key8 : 1, key7 : 1, key6 : 1, key5 : 1,
		key4 : 1, key3 : 1, key2 : 1, key1 : 1;
};

// 共用体使用了内存覆盖技术，同一时刻只能保存一个成员的值，如果对新的成员赋值，就会把原来成员的值覆盖掉。
// num == key1 | key2 | key3 |key4 , 16bits = 4bits * 4
union number {
	int num;
	bit4 numbit4;
	bit16 numbit16;
};


// P31的P盒, mode == 1 代表加密， 0代表解密
void permutation(int mode, number& done) {
	number raw = done;
	if (mode == 1) {
		done.numbit16.key1 = raw.numbit16.key1;
		done.numbit16.key2 = raw.numbit16.key5;
		done.numbit16.key3 = raw.numbit16.key9;
		done.numbit16.key4 = raw.numbit16.key13;
		done.numbit16.key5 = raw.numbit16.key2;
		done.numbit16.key6 = raw.numbit16.key6;
		done.numbit16.key7 = raw.numbit16.key10;
		done.numbit16.key8 = raw.numbit16.key14;
		done.numbit16.key9 = raw.numbit16.key3;
		done.numbit16.key10 = raw.numbit16.key7;
		done.numbit16.key11 = raw.numbit16.key11;
		done.numbit16.key12 = raw.numbit16.key15;
		done.numbit16.key13 = raw.numbit16.key4;
		done.numbit16.key14 = raw.numbit16.key8;
		done.numbit16.key15 = raw.numbit16.key12;
		done.numbit16.key16 = raw.numbit16.key16;
	}
	else {
		done.numbit16.key1 = raw.numbit16.key1;
		done.numbit16.key2 = raw.numbit16.key5;
		done.numbit16.key5 = raw.numbit16.key2;
		done.numbit16.key9 = raw.numbit16.key3;
		done.numbit16.key13 = raw.numbit16.key4;
		done.numbit16.key2 = raw.numbit16.key5;
		done.numbit16.key6 = raw.numbit16.key6;
		done.numbit16.key10 = raw.numbit16.key7;
		done.numbit16.key14 = raw.numbit16.key8;
		done.numbit16.key3 = raw.numbit16.key9;
		done.numbit16.key7 = raw.numbit16.key10;
		done.numbit16.key11 = raw.numbit16.key11;
		done.numbit16.key15 = raw.numbit16.key12;
		done.numbit16.key4 = raw.numbit16.key13;
		done.numbit16.key8 = raw.numbit16.key14;
		done.numbit16.key12 = raw.numbit16.key15;
		done.numbit16.key16 = raw.numbit16.key16;
	}
	return;
}



// 打表
int main() {
	FILE* f;
	f = fopen("permutationSet.txt", "w+");
	if (f == NULL) {
		printf("error!\n");
	}
	printf("open successfully!\n");

	fprintf(f, "mode 0\ndecrypt\n");
	for (int num = 0; num < 65536; ++num) {
		number temp;
		temp.num = num;
		permutation(1, temp);
		if ((num + 1) % 32) {
			fprintf(f, "%d,", temp.num);
		}
		else {
			if (num == 65535) {
				fprintf(f, "%d\n", temp.num);
			}
			else {
				fprintf(f, "%d,\n", temp.num);
			}
		}
	}

	fprintf(f, "mode 1\nencrypt\n");
	for (int num = 0; num < 65536; ++num) {
		number temp;
		temp.num = num;
		permutation(0, temp);
		if ((num + 1) % 32) {
			fprintf(f, "%d,", temp.num);
		}
		else {
			fprintf(f, "%d,\n", temp.num);
		}
	}

	int res = fclose(f);
	if (res == 0) printf("success!");
	else printf("mission fail!");
	system("pause");
	return 0;
}