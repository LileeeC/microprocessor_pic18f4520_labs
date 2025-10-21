#pragma config WDT = OFF
#include <xc.h>
extern unsigned int add(unsigned int a, unsigned int b);
extern unsigned char is_prime(unsigned char n);
extern unsigned int count_primes(unsigned int n, unsigned int m);
extern long mul_extended(int n, int m);

void main(void) {
//    volatile unsigned int result = add(12, 34);
//    volatile unsigned char ans1 = is_prime(253);
    volatile unsigned int ans2 = count_primes(2, 5);
//    volatile long ans3 = mul_extended(32768, 32767);
    while(1);
    return;
}
