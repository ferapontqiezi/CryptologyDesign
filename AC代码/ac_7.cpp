#include <gmp.h>
#include <ctime>
#include <stdio.h>
using namespace std;

mpz_t arr[10000]; // 求逆存为辗转相除数组
char strB[10000]; // 指数存为二进制字符串

// a * a^(-1) = 1 (mod MOD)
inline void GetInverse(mpz_t AInverse, mpz_t A, mpz_t MOD) {
    int count = -1;
    mpz_t tmp, tmp1, tmp2, a, mod;
    mpz_inits(tmp, tmp1, tmp2, NULL); // 初始化
    mpz_init_set(a, A);
    mpz_init_set(mod, MOD);
    // 求解a^(-1)，辗转相除法
	mpz_tdiv_qr(tmp1, tmp, a, mod);  // a = tmp1 * mod + tmp
    mpz_set(arr[0], tmp1);            // arr[0] = tmp1
    while (mpz_cmp_ui(tmp, 0) != 0) {   // tmp != 0
        count++;                       // 记录循环次数count
        mpz_set(arr[count], tmp1);  // arr[count] = tmp1
        mpz_set(a, mod);           // a = mod
        mpz_set(mod, tmp);         // mod = tmp
	    mpz_tdiv_qr(tmp1, tmp, a, mod);  // a = tmp1 * mod + tmp
    }
    mpz_set_ui(tmp1, 0);          // tmp1 = 0
    mpz_set_ui(tmp2, 1);          // tmp2 = 1
    mpz_neg(mod, mod);            // mod = -mod
    for (int i = count; i >= 0; i--) {    // 遍历count次
        mpz_set(tmp, tmp2);         // tmp = tmp2
        mpz_mul(tmp2, tmp2, arr[i]); // tmp2 = tmp2 * arr[i]
        mpz_sub(tmp2, tmp1, tmp2);  // tmp2 = tmp1 - tmp2
        mpz_set(tmp1, tmp);         // tmp1 = tmp
    }
    // gmp_printf("%Zd\n", tmp1);
    mpz_set(AInverse, tmp1);
	mpz_mod(AInverse, AInverse, MOD);
    return ;
}

// Modular repeat square模重复平方法快速求幂 res = a^e(mod n)
inline void FastExpMod(mpz_t res, mpz_t a, mpz_t e, mpz_t n) {
    mpz_get_str(strB, 2, e);        // 将e转化为2进制字符串
	mpz_set_ui(res, 1);
    int i = 0;
	while (strB[i] != '\0') {		// 使用平方乘快速幂
		mpz_mul(res, res, res);
		mpz_mod(res, res, n);       // res = res * res (mod n)
		if (strB[i] == '1') {        // if(num == 1) res *= a (mod n);
			mpz_mul(res, res, a);
			mpz_mod(res, res, n);
		}
		++ i;
	}
    return ;
}


int main() {
    //freopen("10.in", "r", stdin);
    //freopen("10.txt", "w", stdout);
    int cnt;
    // p, q, e, p^(-1) (mod q), q^(-1) (mod p), p-1, q-1, n, phi, d, d(mod p-1), d(mod q-1), 
	mpz_t p, q, e, p_inv, q_inv, p_1, q_1, n, phi, d, d1, d2;
	for (int i = 0; i < 10000; i++) {
		mpz_init(arr[i]);
	}
    mpz_inits(p, q, e, p_inv, q_inv, p_1, q_1, n, phi, d, d1, d2, NULL);
	scanf("%d", &cnt);
    gmp_scanf("%Zd%Zd%Zd", p, q, e);
    // 得到一系列值
    GetInverse(p_inv, p, q), GetInverse(q_inv, q, p);
    mpz_sub_ui(p_1, p, 1), mpz_sub_ui(q_1, q, 1);
    mpz_mul(n, p, q), mpz_mul(phi, p_1, q_1);
    GetInverse(d, e, phi);
    mpz_mod(d1, d, p_1), mpz_mod(d2, d, q_1);

    // m1 = c^(d1)(mod p), m2 = c^(d2)(mod q)
    mpz_t m, m1, m2, c, c1, c2;
    mpz_inits(m, m1, m2, c, c1, c2, NULL);
    while(cnt--){
        gmp_scanf("%Zd", c);
        mpz_powm(m1, c, d1, p), FastExpMod(m2, c, d2, q);
        mpz_mul(m1, m1, q), mpz_mul(m1, m1, q_inv), mpz_mod(m1, m1, n);
        mpz_mul(m2, m2, p), mpz_mul(m2, m2, p_inv), mpz_mod(m2, m2, n);
        mpz_add(m, m1, m2), mpz_mod(m, m, n);
        gmp_printf("%Zd\n", m);
    }

    return 0;
}