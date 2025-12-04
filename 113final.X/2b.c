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
int x = -45;
int direct = 0;
void delay(int sec){
    for(int i = 0; i < sec; i++);
}

void rotate(int angle){
    float p = (angle + 90) / 180.0;
    int value = 2100 * p + 450;
    
    //do things
    CCPR1L = value / 128;
    int r = value % 128;

    if(r < 32) CCP1CONbits.DC1B = 0b00;
    else if(r < 64) CCP1CONbits.DC1B = 0b01;
    else if(r < 96) CCP1CONbits.DC1B = 0b10;
    else CCP1CONbits.DC1B = 0b11;
}

void State(int angle){
    if(direct == 1){
        if(x - angle >= -90){
            x -= angle;
            rotate(x);
        }
        else{
            rotate(-90);
            delay(300);
            x = abs(angle - x) - 180;
            rotate(x);
            direct = 0;
        }
    }
    else{
        if(x + angle <= 90){
            x += angle;
            rotate(x);
        }
        else{
            rotate(90);
            delay(300);
            x = 90 - abs(angle - (90 - x));
            rotate(x);
            direct = 1;
        }
    }
}

void __interrupt()ISR(void){
    if(state == 1){
        State(45);
        state = 2;
    }
    else if(state == 2){
        State(90);
        state = 3;
    }
    else{
        State(180);
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

    // Internal Oscillator Frequency, Fosc = 125 kHz, Tosc = 8 탎
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
     * = (0x9b + 1) * 4 * 8탎 * 4
     * = 0.019968s ~= 20ms
     */
    PR2 = 0x9b;
    
    /**
     * Duty cycle
     * = (CCPR1L:CCP1CON<5:4>) * Tosc * (TMR2 prescaler)
     * = (0x0b*4 + 0b01) * 8탎 * 4
     * = 0.00144s ~= 1450탎
     */
    rotate(x);
    
    PORTB = 0;
    TRISBbits.RB0 = 1;
    
    RCONbits.IPEN = 0; //INT priority disable
    INTCONbits.GIE = 1; //General INT enable
    INTCONbits.PEIE = 0; //Peripheral INT disable
	INTCONbits.INT0IF = 0; //Clear INT0 flag
    INTCONbits.INT0IE = 1; //Enable INT0 INT
    INTCON2bits.INTEDG0 = 0; //Falling edge trigger for INT0
    
    while(1){
    }
    return;
}






