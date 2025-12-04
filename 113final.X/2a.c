#include <xc.h>
#include <pic18f4520.h>

#pragma config OSC = INTIO67    // Oscillator Selection bits
#pragma config WDT = OFF        // Watchdog Timer Enable bit 
#pragma config PWRT = OFF       // Power-up Enable bit
#pragma config BOREN = ON       // Brown-out Reset Enable bit
#pragma config PBADEN = OFF     // Watchdog Timer Enable bit 
#pragma config LVP = OFF        // Low Voltage (single -supply) In-Circute Serial Pragramming Enable bit
#pragma config CPD = OFF        // Data EEPROM?Memory Code Protection bit (Data EEPROM code protection off)

int state = 1;

void delay(int sec){
    for(int i = 0; i < sec; i++);
}
void __interrupt()ISR(void){
    if(state == 1){
        CCPR1L = 0x13;
        CCP1CONbits.DC1B = 0b01;
        state = 2;
    }
    else if(state == 2){
        CCPR1L = 0x13;
        CCP1CONbits.DC1B = 0b01;
        state = 1;
    }
    delay(100);
    INTCONbits.INT0IF = 0;
    return;
} 

void main(void)
{
    // Timer2 -> On, prescaler -> 4
    T2CONbits.TMR2ON = 0b1;
    T2CONbits.T2CKPS = 0b01;

    // Internal Oscillator Frequency, Fosc = 125 kHz, Tosc = 8 µs
    OSCCONbits.IRCF = 0b001;
    
    // PWM mode, P1A, P1C active-high; P1B, P1D active-high
    CCP1CONbits.CCP1M = 0b1100;
    
    // CCP1/RC2 -> Output
    TRISC = 0;
    LATC = 0;
    
    // Set up PR2, CCP to decide PWM period and Duty Cycle
    /** 
     * PWM period
     * = (PR2 + 1) * 4 * Tosc * (TMR2 prescaler)
     * = (0x9b + 1) * 4 * 8µs * 4
     * = 0.019968s ~= 20ms
     */
    PR2 = 0x9b;
    
    /**
     * Duty cycle
     * = (CCPR1L:CCP1CON<5:4>) * Tosc * (TMR2 prescaler)
     * = (0x0b*4 + 0b01) * 8µs * 4
     * = 0.00144s ~= 1450µs
     */
    CCPR1L = 0x13;
    CCP1CONbits.DC1B = 0b01;
    
    PORTB = 0;
    TRISBbits.RB0 = 1;
    
    RCONbits.IPEN = 0; //INT priority disable
    INTCONbits.GIE = 1; //General INT enable
    INTCONbits.PEIE = 0; //Peripheral INT disable
	INTCONbits.INT0IF = 0; //Clear INT0 flag
    INTCONbits.INT0IE = 1; //Enable INT0 INT
    INTCON2bits.INTEDG0 = 0; //Falling edge trigger for INT0
    
    while(1){
        if(state == 1){
            for(int i = 0x13; i >= 0x03; i--){
                CCPR1L = i;
                for(int j = 0x03; j >= 0x01; j--){
                    CCP1CONbits.DC1B = j;
                    delay(40);
                }
            }
            delay(500);
            CCPR1L = 0x13;
            CCP1CONbits.DC1B = 0b11;
            delay(600);
        }
        else if(state == 2){
            for(int i = 0x13; i >= 0x0b; i--){
                CCPR1L = i;
                for(int j = 0x03; j >= 0x01; j--){
                    CCP1CONbits.DC1B = j;
                    delay(40);
                }
            }
            delay(500);
            CCPR1L = 0x13;
            CCP1CONbits.DC1B = 0b11;
            delay(600);
        }
    }
    return;
}





