#include <xc.h>
#include <pic18f4520.h>
#include <stdio.h>

#pragma config OSC = INTIO67 // Oscillator Selection bits
#pragma config WDT = OFF     // Watchdog Timer Enable bit
#pragma config PWRT = OFF    // Power-up Enable bit
#pragma config BOREN = ON    // Brown-out Reset Enable bit
#pragma config PBADEN = OFF  // Watchdog Timer Enable bit
#pragma config LVP = OFF     // Low Voltage (single -supply) In-Circute Serial Pragramming Enable bit
#pragma config CPD = OFF     // Data EEPROM?Memory Code Protection bit (Data EEPROM code protection off)

#define _XTAL_FREQ 1000000 // ???? 8MHz ???

int value = 0;
void __interrupt(high_priority)H_ISR(){

    //step4
    
    value = (ADRESH << 2) | (ADRESL >> 6);

    //clear flag bit
    PIR1bits.ADIF = 0;
    
    return;
}

void __interrupt(low_priority)L_ISR(){
    return;
}
void main(void) 
{   
     // Timer2 -> On, prescaler -> 16
    T2CONbits.TMR2ON = 0b1;
    T2CONbits.T2CKPS = 0b10;

    // Internal Oscillator Frequency, Fosc = 1 MHz, Tosc = 1 µs
    OSCCONbits.IRCF = 0b100; 
    
    // PWM mode, P1A, P1C active-high; P1B, P1D active-high
    CCP1CONbits.CCP1M = 0b1100;
    
    // CCP1/RC2 -> Output
    TRISC = 0;
    LATC = 0;
    
    // Set up PR2, CCP to decide PWM period and Duty Cycle
    /*
     * PWM period
     * = (PR2 + 1) * 4 * Tosc * (TMR2 prescaler)
     * = (0x9B + 1) * 4 * 1µs * 16
     *  ~= 10ms
     */
    PR2 = 0x9B;
    
    /*
     * Duty cycle
     * = (CCPR1L:CCP1CON<5:4>) * Tosc * (TMR2 prescaler)
     */
    CCPR1L = 0x00;
    CCP1CONbits.DC1B = 0b00;
    
    //configure OSC and port
    //OSCCONbits.IRCF = 0b100; //1MHz 
    TRISAbits.RA0 = 1;       //analog input port
    
    //step1
    ADCON1bits.VCFG0 = 0;
    ADCON1bits.VCFG1 = 0;
    ADCON1bits.PCFG = 0b1110; //AN0 ?analog input,???? digital
    ADCON0bits.CHS = 0b0000;  //AN0 ?? analog input
    ADCON2bits.ADCS = 0b000;  //????000(1Mhz < 2.86Mhz)
    ADCON2bits.ACQT = 0b001;  //Tad = 2 us acquisition time?2Tad = 4 > 2.4
    ADCON0bits.ADON = 1;
    ADCON2bits.ADFM = 0;    //left justified 
    
    
    //step2
    PIE1bits.ADIE = 1;
    PIR1bits.ADIF = 0;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    

    //step3
    ADCON0bits.GO = 1;

    while(1){
        CCPR1L = value >> 2;
        CCP1CONbits.DC1B = value & 0b11;
        __delay_us(4);
        ADCON0bits.GO = 1;
    };
    
    
    return;
}
