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
#define _XTAL_FREQ 4000000 
int count = 0;
int x = 1;
int t = 1;
int state = 1;
int timer = 0;

void __interrupt(high_priority)H_ISR(){
    if(state == 1){
        timer = 1;
        state++;
    }
    else if(state == 2){
        timer = 3;
        state++;
    }
    else{
        timer = 0;
        state = 1;
    }
    if(count > timer) count = 0;
    __delay_us(10);
    INTCONbits.INT0IF = 0;
    return;
}

void __interrupt(low_priority)L_ISR(){
    if(count == timer){
        if(x == 1){
            LATA = 0x01 << 1;
            x++;
        }
        else if(x == 2){
            LATA = 0x02 << 1;
            x++;
        }
        else{
            LATA = 0x04 << 1;
            x = 1;
        }
        count = 0;
    }
    else count++;
    PIR1bits.TMR2IF = 0;
    return;
}

void Timer2_Initialize(){
    // Timer2 -> On, prescaler -> 4
    //T2CONbits.TMR2ON = 0b1;
    //T2CONbits.T2CKPS = 0b11;
    PIR1bits.TMR2IF = 0;
    PIE1bits.TMR2IE = 1;
    IPR1bits.TMR2IP = 0;
    T2CON = 0xFF;
    PR2 = 0xFF;
}
void main(void) 
{
    OSCCONbits.IRCF = 0b100; //1MHz
    Timer2_Initialize();
    
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    RCONbits.IPEN = 1;
    
    ADCON1bits.PCFG = 0b1111;
    TRISAbits.RA1 = 0;
    TRISAbits.RA2 = 0;
    TRISAbits.RA3 = 0;
    LATA = x << 1;
    PORTB = 0;
    TRISBbits.RB0 = 1;
    
	INTCONbits.INT0IF = 0; //Clear INT0 flag
    INTCONbits.INT0IE = 1; //Enable INT0 INT
    INTCON2bits.INTEDG0 = 0; //Falling edge trigger for INT0
    
    T2CONbits.TMR2ON = 1;
    while(1){};
    
    return;
}


