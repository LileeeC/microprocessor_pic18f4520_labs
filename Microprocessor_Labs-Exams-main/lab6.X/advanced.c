#include <xc.h>
#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF

// ??????? __delay_ms()
#define _XTAL_FREQ 8000000 // ???? 8MHz ???

extern void blink(unsigned char pattern);

unsigned char pattern = 0;
unsigned char null_pattern = 0;


void main(void) {
    ADCON1 = 0x0F;
    LATA = 0x00; 
    PORTA = 0x00;  // ???????
    LATB = 0x00; 
    PORTB = 0x00; 
    TRISA = 0b00000000; 
    TRISB = 0b00000001; 
    
    while(1){
        if(PORTBbits.RB0 == 0){
            pattern++;
            blink(pattern);
            pattern++;
            blink(pattern);
            pattern++;
            blink(pattern);
            pattern++;
            pattern++;
            blink(pattern);
            pattern = 0;
            blink(pattern);
        }
    }
    return;
}
