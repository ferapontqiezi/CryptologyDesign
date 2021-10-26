#include <gmp.h>
#include <ctime>
#include <stdio.h>
using namespace std;

char strB[10000]; // 指数存为二进制字符串
// Modular repeat square模重复平方法快速求幂 res = a^e(mod n)
inline void FastExpMod(mpz_t res, mpz_t a, mpz_t e, mpz_t n) {
    mpz_get_str(strB, 2, e);        // 将e转化为2进制字符串
    // printf("%s\n", strB);
	mpz_set_ui(res, 1);
    int i = 0;
	while (strB[i] != '\0') {		// 使用平方乘快速幂
		mpz_mul(res, res, res);
		mpz_mod(res, res, n);       // res = res * res (mod n)
		if (strB[i] == '1') {        // if(num == 1) res *= a (mod n);
			mpz_mul(res, res, a);
			mpz_mod(res, res, n);
		}
        i++;
	}
    return ;
}

int main() {
    //freopen("6.in", "r", stdin);
    //freopen("6.txt", "w", stdout);
    int cnt;
    scanf("%d", &cnt);
    while(cnt--){
        mpz_t c, m, e, n, p, q;
        mpz_inits(c, m, e, n, p, q, NULL);
        gmp_scanf("%Zd%Zd%Zd%Zd", e, m, p, q);
        mpz_mul(n,p,q);
        FastExpMod(c, m, e, n);
        gmp_printf("%Zd\n", c);
    }
    return 0;
}