#include <gmp.h>
#include <ctime>
#include <stdio.h>
using namespace std;

mpz_t st[10000]; // 求逆存为辗转相除数组
char str[10000]; // 指数存为二进制字符串

// a * a^(-1) = 1 (mod MOD)
inline void GetInverse(mpz_t AInverse, mpz_t A, mpz_t MOD) {
    int count = -1;
    mpz_t tmp, tmp1, tmp2, a, mod;
    mpz_inits(tmp, tmp1, tmp2, NULL); // 初始化
    mpz_init_set(a, A);
    mpz_init_set(mod, MOD);
    // 求解a^(-1)，辗转相除法
	mpz_tdiv_qr(tmp1, tmp, a, mod);  // a = tmp1 * mod + tmp
    mpz_set(st[0], tmp1);            // st[0] = tmp1
    while (mpz_cmp_ui(tmp, 0) != 0) {   // tmp != 0
        count++;                       // 记录循环次数count
        mpz_set(st[count], tmp1);  // st[count] = tmp1
        mpz_set(a, mod);           // a = mod
        mpz_set(mod, tmp);         // mod = tmp
	    mpz_tdiv_qr(tmp1, tmp, a, mod);  // a = tmp1 * mod + tmp
    }
    mpz_set_ui(tmp1, 0);          // tmp1 = 0
    mpz_set_ui(tmp2, 1);          // tmp2 = 1
    mpz_neg(mod, mod);            // mod = -mod
    for (int i = count; i >= 0; i--) {    // 遍历count次
        mpz_set(tmp, tmp2);         // tmp = tmp2
        mpz_mul(tmp2, tmp2, st[i]); // tmp2 = tmp2 * st[i]
        mpz_sub(tmp2, tmp1, tmp2);  // tmp2 = tmp1 - tmp2
        mpz_set(tmp1, tmp);         // tmp1 = tmp
    }
    // gmp_printf("%Zd\n", tmp1);
    mpz_set(AInverse, tmp1);
	mpz_mod(AInverse, AInverse, MOD);
    mpz_clears(tmp, tmp1, tmp2, a, mod, NULL);
    return ;
}

// Modular repeat square模重复平方法快速求幂 res = a^e(mod n)
inline void FastExpMod(mpz_t res, mpz_t a, mpz_t e, mpz_t n) {
    mpz_get_str(str, 2, e);        // 将e转化为2进制字符串
	mpz_set_ui(res, 1);
    int i = 0;
	while (str[i] != '\0') {		// 使用平方乘快速幂
		mpz_mul(res, res, res);
		mpz_mod(res, res, n);       // res = res * res (mod n)
		if (str[i] == '1') {        // if(num == 1) res *= a (mod n);
			mpz_mul(res, res, a);
			mpz_mod(res, res, n);
		}
		++ i;
	}
    return ;
}

// Chinese Remainder Theory中国剩余定理求RSA加密和解密
inline void CRT_decr(mpz_t m, mpz_t d, mpz_t p, mpz_t q, mpz_t c) {
    mpz_t m1, m2, p_1, q_1, d1, d2, n, phi, p_inv, q_inv;
    mpz_inits(m1, m2, p_1, q_1, d1, d2, n, phi, p_inv, q_inv, NULL);
    mpz_sub_ui(p_1, p, 1); mpz_sub_ui(q_1, q, 1);
    mpz_mul(n, p, q); mpz_mul(phi, p_1, q_1);
    GetInverse(d1, d, p_1); GetInverse(d2, d, q_1);
    FastExpMod(m1, c, d1, p); FastExpMod(m2, c, d2, q);
    mpz_mul(m1, m1, q); mpz_mul(m2, m2, p);
    GetInverse(p_inv, p, q); GetInverse(q_inv, q, p);
    mpz_mul(m1, m1, p_inv); mpz_mul(m2, m2, q_inv);
    mpz_add(m, m1, m2);
    mpz_mod(m, m, n);
    return ;
} 

inline void CRT_encr(mpz_t c, mpz_t e, mpz_t p, mpz_t q, mpz_t m) {
    mpz_t c1, c2, p_1, q_1, e1, e2, n, phi, p_inv, q_inv;
    mpz_inits(c1, c2, p_1, q_1, e1, e2, n, phi, p_inv, q_inv, NULL);
    mpz_sub_ui(p_1, p, 1); mpz_sub_ui(q_1, q, 1);
    mpz_mul(n, p, q); mpz_mul(phi, p_1, q_1);
    GetInverse(e1, e, p_1); GetInverse(e2, e, q_1);
    FastExpMod(c1, m, e1, p); FastExpMod(c2, m, e2, q);
    mpz_mul(c1, c1, q); mpz_mul(c2, c2, p);
    GetInverse(p_inv, p, q); GetInverse(q_inv, q, p);
    mpz_mul(c1, c1, p_inv); mpz_mul(c2, c2, q_inv);
    mpz_add(c, c1, c2);
    mpz_mod(c, c, n);
    return ;
}


int main() {
    //freopen("8.in", "r", stdin);
    //freopen("8.txt", "w", stdout);
    int cnt;
    // p, q, e, p^(-1) (mod q), q^(-1) (mod p), p-1, q-1, n, phi, d, d(mod p-1), d(mod q-1), 
	mpz_t p, q, e, p_inverse, q_inverse, p_1, q_1, n, phi, d, d1, d2;
	for (int i = 0; i < 1000; i++) {
		mpz_init(st[i]);
	}
    mpz_inits(p, q, e, p_inverse, q_inverse, p_1, q_1, n, phi, d, d1, d2, NULL);
	scanf("%d", &cnt);
    gmp_scanf("%Zd%Zd%Zd", p, q, e);
    // 得到一系列值
    GetInverse(p_inverse, p, q);
    GetInverse(q_inverse, q, p);
    mpz_sub_ui(p_1, p, 1);
    mpz_sub_ui(q_1, q, 1);
    mpz_mul(n, p, q);
    mpz_mul(phi, p_1, q_1);
    GetInverse(d, e, phi);
    mpz_mod(d1, d, p_1);
    mpz_mod(d2, d, q_1);

    // m1 = c^(d1)(mod p), m2 = c^(d2)(mod q)
    mpz_t m, m1, m2, c, c1, c2;
    mpz_inits(m, m1, m2, c, c1, c2, NULL);
    while(cnt){
        gmp_scanf("%Zd", m);
        FastExpMod(c1, m, d1, p);
        FastExpMod(c2, m, d2, q);
        
        gmp_printf("%Zd", c);
        -- cnt;
    }

	// while (n--) {
	// 	gmp_scanf("%Zd", m);
	// 	mpz_set(b1, m);
	// 	mpz_set(b2, m);
	// 	expmod(b1, d1, p);		// 分别计算m^d1和m^d2
	// 	expmod(b2, d2, q);


	// 	mpz_mul(b1, b1, q);		// 运用中国剩余定理计算
	// 	mpz_mod(b1, b1, B);
	// 	mpz_mul(b1, b1, qInverse);
	// 	mpz_mod(b1, b1, B);

	// 	mpz_mul(b2, b2, p);
	// 	mpz_mod(b2, b2, B);
	// 	mpz_mul(b2, b2, pInverse);
	// 	mpz_mod(b2, b2, B);

	// 	mpz_add(b1, b1, b2);
	// 	mpz_mod(b1, b1, B);
	// 	gmp_printf("%Zd\n", b1);
    
    //mpz_clears(d, e, p, q, NULL);
    return 0;
}