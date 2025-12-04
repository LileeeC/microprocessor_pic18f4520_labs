#include <xc.h>
#pragma config WDT = OFF

/*extern unsigned char is_prime(unsigned char n);

void main(void) {
    volatile unsigned char ans = is_prime(253);
    while(1);
    return;
}*/

/*extern unsigned int count_primes(unsigned int n, unsigned int m);

void main(void) {
    volatile unsigned int ans = count_primes(1, 256);
    while(1);
    return;
}*/

extern long mul_extended(int n, int m);

void main(void) {
    volatile long ans = mul_extended(-32768, 32767);
    while(1);
    return;
}
