#include <gmp.h>
#include <ctime>
#include <stdio.h>
using namespace std;

// 理论知识：https://blog.nowcoder.net/n/0b00620446d647749883aee852da5382
// 用Miller-Rabin素性检测 检测n是否为素数
bool MillerRabinPrime(mpz_t n){
	gmp_randstate_t state;
	gmp_randinit_default(state);         //对state进行初始化
	gmp_randseed_ui(state, time(NULL));  //对state置初始种子，以时间作为种子

	mpz_t m, a, b, flag, n_1;
	mpz_inits(m, a, b, flag, n_1, NULL);  //初始化

	mpz_sub_ui(m, n, 1);      // m = n - 1
	mpz_mod_ui(flag, m, 2);   // flag = m % 2
	mpz_sub_ui(n_1, n, 1);    // n_1 = n-1 = -1 mod n

	//下面计算二次探测的最大次数, φ(n) = n - 1 = 2^r*m, m为奇数
	int r = 0;
	//计算r, 将n-1表示成m*2^r ,s*2^t
	while (!mpz_tstbit(m,1)){  //测试最后一位，为1说明是奇数，终止循环。为0则说明是偶数，继续循环
		mpz_tdiv_q_2exp(m, m, 1);    //m右移一位, 即m=m/2
		r++;
	}
	//随机生成一个[1,n-1]之间的随机数
	mpz_urandomm(a, state, n_1);   //产生随机数a, 0<=a<=n-2
	mpz_add_ui(a, a, 1);  //a=a+1,此时1<=a<=n-1      
    mpz_powm(b, a, m, n);           //计算出b=a^m mod n
	if (!mpz_cmp_ui(b, 1)){    //若a^m=1, 则说明通过二次探测, 直接返回
		mpz_clears(m, flag, n_1, a, b,NULL);  //清理申请的大数空间
		gmp_randclear(state);
		return true;
	}
	//n-1表示成m*2^r，如果n是一个素数，那么或者a^m mod n=1，
	//或者存在某个i使得a^(m*2^i) mod n=n-1 ( 0<=i<r )
	for (int i = 0; i < r; i++){
		if (!mpz_cmp(b, n_1)){   //若b=n-1,说明符合二次探测，返回true
			mpz_clears(m, flag, n_1, a, b, NULL);
			gmp_randclear(state);
			return true;
		}
		else{
			mpz_mul(b, b, b);//b=b^2;
			mpz_mod(b, b, n);//b=b mod n;
		}
	}

	mpz_clears(m, flag, n_1, a, b, NULL); // 清理大数空间
	gmp_randclear(state); // 清理状态
	return false;
}

// 判断p，q互素
bool RelativelyPrime(mpz_t p, mpz_t q) {
    mpz_t a, b, d;
    mpz_init(d);
    mpz_init_set(a, p);
    mpz_init_set(b, q);

    mpz_mod(d, a, b); // d = a (mod b)
    // 辗转相除法
    while(mpz_cmp_ui(d, 0) != 0) { // d != 0一直循环
        mpz_init_set(a, b); // a = b
        mpz_init_set(b, d); // b = d
        mpz_mod(d, a, b); // d = a (mod b)
    }
    mpz_clears(a, d, NULL);
    if(mpz_cmp_ui(b, 1) == 0) { // 如果d=0，b=1则p与q互素；不然不互素
        mpz_clear(b);
        return true;
    }
    mpz_clear(b);
    return false;
}

// pollard（p-1）分解攻击，需要p-1和q-1的最大公约数很大
bool HaveBigGcd(mpz_t p, mpz_t q) {
    // 在互素上改最后的判断即可
    mpz_t a, b, d;
    mpz_init(d);
    mpz_init_set(a, p);
    mpz_init_set(b, q);

    mpz_mod(d, a, b); // d = a (mod b)
    // 辗转相除法
    while(mpz_cmp_ui(d, 0) != 0) { // d != 0一直循环
        mpz_init_set(a, b); // a = b
        mpz_init_set(b, d); // b = d
        mpz_mod(d, a, b); // d = a (mod b)
    }
    mpz_clears(a, d, NULL);
    if(mpz_cmp_ui(b, 1e5) > 0) { // 如果b > 100000，则认为p-1和q-1的最大公约数很大
        mpz_clear(b);
        return true;
    }
    mpz_clear(b);
    return false;
}

// 得到参数d，如果有参数不合适，则输出ERROR
void GetParameter(mpz_t e, mpz_t p, mpz_t q) {
    mpz_t st[4000];
    int count = -1;
    for (int i = 0; i < 4000; i++) {
        mpz_init(st[i]);
    }
    // phi是phi(n), phiP是phi(p)...
    mpz_t phi, phiP, phiQ, tmp, tmp1, tmp2;
    mpz_inits(phi, phiP, phiQ, tmp, tmp1, tmp2, NULL); // 初始化
    mpz_sub(tmp, p, q);       // tmp = p - q
    mpz_div_ui(tmp1, p, 10);  // tmp1 = p / 10
    mpz_abs(tmp, tmp);        // tmp = abs(tmp) = abs(p-q)
    // 各种攻击：https://blog.csdn.net/pigeon_1/article/details/114371456
    if (mpz_cmp_ui(e, 100) < 0) {                // e过小
        printf("ERROR\n");
        return;
    }
    if ( (!MillerRabinPrime(p)) || (!MillerRabinPrime(q)) ) {    // p, q有一个不是是素数
        printf("ERROR\n");
        return;
    }
    // 0.9*p < q < 1.1*p
    if (mpz_cmp(tmp, tmp1) < 0) {              // p,q差值过小，费马分解法容易破解e
        printf("ERROR\n");
        return;
    }
    mpz_sub_ui(phiP, p, 1);   // phiP = p - 1
    mpz_sub_ui(phiQ, q, 1);   // phiQ = q - 1
    if (HaveBigGcd(phiP, phiQ)) {                    // p-1和q-1的gcd太大，pollard（p-1）分解攻击
        printf("ERROR\n");
        return;
    }
    mpz_mul(phi, phiP, phiQ);  // phi = (p-1) * (q-1) = phiP * phiQ
    mpz_neg(phi, phi);         // phi = -phi
    if (!RelativelyPrime(e, phi)) {            // e和phi不互素
        printf("ERROR\n");
        return;
    }
    // 求解d，辗转相除法
    // RSA_Parameters: d*e = 1 (mod phi)
	mpz_tdiv_qr(tmp1, tmp, e, phi);  // e = tmp1 * phi + tmp
    mpz_set(st[0], tmp1);       // st[0] = tmp1 --------> st[0] = k
    while (mpz_cmp_ui(tmp, 0) != 0) {   // tmp != 0
        count++;                       // 记录循环次数count
        mpz_set(st[count], tmp1);  // st[count] = tmp1
        mpz_set(e, phi);           // e = phi
        mpz_set(phi, tmp);         // phi = tmp
	    mpz_tdiv_qr(tmp1, tmp, e, phi);  // e = tmp1 * phi + tmp
    }
    mpz_set_ui(tmp1, 0);          // tmp1 = 0
    mpz_set_ui(tmp2, 1);          // tmp2 = 1
    mpz_neg(phi, phi);            // phi = -phi
    for (int i = count; i >= 0; i--) {    // 遍历count次
        mpz_set(tmp, tmp2);         // tmp = tmp2
        mpz_mul(tmp2, tmp2, st[i]); // tmp2 = tmp2 * st[i]
        mpz_sub(tmp2, tmp1, tmp2);  // tmp2 = tmp1 - tmp2
        mpz_set(tmp1, tmp);         // tmp1 = tmp
    }
    gmp_printf("%Zd\n", tmp1);
    for (int i = 0; i < 4000; i++) {
        mpz_clear(st[i]);
    }
    mpz_clears(phi, phiP, phiQ, tmp, tmp1, tmp2, NULL);
    return ;
}

int main() {
    //freopen("E:\\EXAMPLE5\\4.in", "r", stdin);
    //freopen("E:\\EXAMPLE5\\4.txt", "w", stdout);
    mpz_t e, p, q;
    mpz_inits(e, p, q, NULL);
    int n;
    scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        gmp_scanf("%Zd%Zd%Zd", e, p, q);
        GetParameter(e, p, q);
    }
    mpz_clears(e, p, q, NULL);
    return 0;
}