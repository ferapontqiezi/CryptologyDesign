#include <gmp.h>
#include <ctime>
#include <stdio.h>
using namespace std;

// ����֪ʶ��https://blog.nowcoder.net/n/0b00620446d647749883aee852da5382
// ��Miller-Rabin���Լ�� ���n�Ƿ�Ϊ����
bool MillerRabinPrime(mpz_t n){
	gmp_randstate_t state;
	gmp_randinit_default(state);         //��state���г�ʼ��
	gmp_randseed_ui(state, time(NULL));  //��state�ó�ʼ���ӣ���ʱ����Ϊ����

	mpz_t m, a, b, flag, n_1;
	mpz_inits(m, a, b, flag, n_1, NULL);  //��ʼ��

	mpz_sub_ui(m, n, 1);      // m = n - 1
	mpz_mod_ui(flag, m, 2);   // flag = m % 2
	mpz_sub_ui(n_1, n, 1);    // n_1 = n-1 = -1 mod n

	//����������̽���������, ��(n) = n - 1 = 2^r*m, mΪ����
	int r = 0;
	//����r, ��n-1��ʾ��m*2^r ,s*2^t
	while (!mpz_tstbit(m,1)){  //�������һλ��Ϊ1˵������������ֹѭ����Ϊ0��˵����ż��������ѭ��
		mpz_tdiv_q_2exp(m, m, 1);    //m����һλ, ��m=m/2
		r++;
	}
	//�������һ��[1,n-1]֮��������
	mpz_urandomm(a, state, n_1);   //���������a, 0<=a<=n-2
	mpz_add_ui(a, a, 1);  //a=a+1,��ʱ1<=a<=n-1      
    mpz_powm(b, a, m, n);           //�����b=a^m mod n
	if (!mpz_cmp_ui(b, 1)){    //��a^m=1, ��˵��ͨ������̽��, ֱ�ӷ���
		mpz_clears(m, flag, n_1, a, b,NULL);  //��������Ĵ����ռ�
		gmp_randclear(state);
		return true;
	}
	//n-1��ʾ��m*2^r�����n��һ����������ô����a^m mod n=1��
	//���ߴ���ĳ��iʹ��a^(m*2^i) mod n=n-1 ( 0<=i<r )
	for (int i = 0; i < r; i++){
		if (!mpz_cmp(b, n_1)){   //��b=n-1,˵�����϶���̽�⣬����true
			mpz_clears(m, flag, n_1, a, b, NULL);
			gmp_randclear(state);
			return true;
		}
		else{
			mpz_mul(b, b, b);//b=b^2;
			mpz_mod(b, b, n);//b=b mod n;
		}
	}

	mpz_clears(m, flag, n_1, a, b, NULL); // ��������ռ�
	gmp_randclear(state); // ����״̬
	return false;
}

// �ж�p��q����
bool RelativelyPrime(mpz_t p, mpz_t q) {
    mpz_t a, b, d;
    mpz_init(d);
    mpz_init_set(a, p);
    mpz_init_set(b, q);

    mpz_mod(d, a, b); // d = a (mod b)
    // շת�����
    while(mpz_cmp_ui(d, 0) != 0) { // d != 0һֱѭ��
        mpz_init_set(a, b); // a = b
        mpz_init_set(b, d); // b = d
        mpz_mod(d, a, b); // d = a (mod b)
    }
    mpz_clears(a, d, NULL);
    if(mpz_cmp_ui(b, 1) == 0) { // ���d=0��b=1��p��q���أ���Ȼ������
        mpz_clear(b);
        return true;
    }
    mpz_clear(b);
    return false;
}

// pollard��p-1���ֽ⹥������Ҫp-1��q-1�����Լ���ܴ�
bool HaveBigGcd(mpz_t p, mpz_t q) {
    // �ڻ����ϸ������жϼ���
    mpz_t a, b, d;
    mpz_init(d);
    mpz_init_set(a, p);
    mpz_init_set(b, q);

    mpz_mod(d, a, b); // d = a (mod b)
    // շת�����
    while(mpz_cmp_ui(d, 0) != 0) { // d != 0һֱѭ��
        mpz_init_set(a, b); // a = b
        mpz_init_set(b, d); // b = d
        mpz_mod(d, a, b); // d = a (mod b)
    }
    mpz_clears(a, d, NULL);
    if(mpz_cmp_ui(b, 1e5) > 0) { // ���b > 100000������Ϊp-1��q-1�����Լ���ܴ�
        mpz_clear(b);
        return true;
    }
    mpz_clear(b);
    return false;
}

// �õ�����d������в��������ʣ������ERROR
void GetParameter(mpz_t e, mpz_t p, mpz_t q) {
    mpz_t st[4000];
    int count = -1;
    for (int i = 0; i < 4000; i++) {
        mpz_init(st[i]);
    }
    // phi��phi(n), phiP��phi(p)...
    mpz_t phi, phiP, phiQ, tmp, tmp1, tmp2;
    mpz_inits(phi, phiP, phiQ, tmp, tmp1, tmp2, NULL); // ��ʼ��
    mpz_sub(tmp, p, q);       // tmp = p - q
    mpz_div_ui(tmp1, p, 10);  // tmp1 = p / 10
    mpz_abs(tmp, tmp);        // tmp = abs(tmp) = abs(p-q)
    // ���ֹ�����https://blog.csdn.net/pigeon_1/article/details/114371456
    if (mpz_cmp_ui(e, 100) < 0) {                // e��С
        printf("ERROR\n");
        return;
    }
    if ( (!MillerRabinPrime(p)) || (!MillerRabinPrime(q)) ) {    // p, q��һ������������
        printf("ERROR\n");
        return;
    }
    // 0.9*p < q < 1.1*p
    if (mpz_cmp(tmp, tmp1) < 0) {              // p,q��ֵ��С������ֽⷨ�����ƽ�e
        printf("ERROR\n");
        return;
    }
    mpz_sub_ui(phiP, p, 1);   // phiP = p - 1
    mpz_sub_ui(phiQ, q, 1);   // phiQ = q - 1
    if (HaveBigGcd(phiP, phiQ)) {                    // p-1��q-1��gcd̫��pollard��p-1���ֽ⹥��
        printf("ERROR\n");
        return;
    }
    mpz_mul(phi, phiP, phiQ);  // phi = (p-1) * (q-1) = phiP * phiQ
    mpz_neg(phi, phi);         // phi = -phi
    if (!RelativelyPrime(e, phi)) {            // e��phi������
        printf("ERROR\n");
        return;
    }
    // ���d��շת�����
    // RSA_Parameters: d*e = 1 (mod phi)
	mpz_tdiv_qr(tmp1, tmp, e, phi);  // e = tmp1 * phi + tmp
    mpz_set(st[0], tmp1);       // st[0] = tmp1 --------> st[0] = k
    while (mpz_cmp_ui(tmp, 0) != 0) {   // tmp != 0
        count++;                       // ��¼ѭ������count
        mpz_set(st[count], tmp1);  // st[count] = tmp1
        mpz_set(e, phi);           // e = phi
        mpz_set(phi, tmp);         // phi = tmp
	    mpz_tdiv_qr(tmp1, tmp, e, phi);  // e = tmp1 * phi + tmp
    }
    mpz_set_ui(tmp1, 0);          // tmp1 = 0
    mpz_set_ui(tmp2, 1);          // tmp2 = 1
    mpz_neg(phi, phi);            // phi = -phi
    for (int i = count; i >= 0; i--) {    // ����count��
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