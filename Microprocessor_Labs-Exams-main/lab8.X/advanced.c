// PIC18F4520 Configuration Bit Settings
// 'C' source line config statements

#pragma config OSC = INTIO67    // Oscillator Selection bits (Internal oscillator block, port function on RA6 and RA7)
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bits (Brown-out Reset enabled and controlled by software (SBOREN is enabled))
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <pic18f4520.h>

#define _XTAL_FREQ 8000000 // ???? 8MHz ???

int steps = -1; //3 steps, 45 degreee per step, -1 if idle
// 20 steps, 


void __interrupt() ISR(void)
{
    INTCONbits.INT0IE = 0;          // ???? INT0?????????
    steps = 0;
    __delay_ms(20);     // ?? 0.02 ??????

    INTCONbits.INT0IF = 0;          // ??????
    INTCONbits.INT0IE = 1;          // ???? INT0
}

void main(void)
{
    //set up
    ADCON1 = 0x0F;
    
    TRISBbits.RB0 = 1; //set RB0 as input
    
    RCONbits.IPEN = 0 ; //disable priority
    INTCONbits.GIE = 1; //enable global interrupt
    INTCONbits.INT0IF = 0; //clear interrupt0 flag
    INTCONbits.INT0IE = 1; //enable interrupt0
    
    INTCON2bits.INTEDG0 = 1; // rising edge triggered (released)
    
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
    /*
     * PWM period
     * = (PR2 + 1) * 4 * Tosc * (TMR2 prescaler)
     * = (0x9B + 1) * 4 * 8µs * 4
     * = 0.019968s ~= 20ms
     */
    PR2 = 0x9B;
    
    CCPR1L = 0x04;
    CCP1CONbits.DC1B = 0b00; //-90

    int duty_cycle_bits = 0b0000010000;
    int direction = 1;

    while(1){

        if(steps != -1){
            if(direction == 1){
                duty_cycle_bits += 3;
                if(duty_cycle_bits >= 76){
                    direction = 0;
                }
            }
            else{
                duty_cycle_bits -= 3;
                if(duty_cycle_bits <= 16){
                    direction = 1;
                }

            }
            
            CCPR1L = duty_cycle_bits >> 2;
            CCP1CONbits.DC1B = duty_cycle_bits & 0b11;
            __delay_ms(1);     

            steps++;
            if(steps > 14){
                steps = -1;
            }
        }
    }
}