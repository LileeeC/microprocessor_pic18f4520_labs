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

#define _XTAL_FREQ 125000 // ???? Fosc (Hz), for __delay_ms()

#define NEG_90 31  // -90 ? (10-bit value)
#define POS_90 63  // +90 ? (10-bit value)

// Timer0 ? 32 탎 ?? 1 ??? 256 ??overflow???? 
// ???? = 32 탎 * 256 = 8,192 탎 = 8.192ms?????????
// (63-31)*2 = 64 ?
// 5 ? / 64 ? = 5000 ms / 64 ? ? 78 ms/?
// TARGET = 78 ms / 8.192 ms ? 9.52
// ??????????? 10 * 8.192ms ? 81.9ms)
// 64 * 81.9ms ? 5.2 ? (approximately 5 seconds)
#define TIMER_TARGET 10

volatile char isRunning = 0;
volatile signed int current;
volatile signed char dir = 1;
volatile unsigned char timer_counter = 0; // ??????

void set_motor_led(unsigned int value){
    // CCP1 (motor)
    CCPR1L = value >> 2;
    CCP1CONbits.DC1B = value & 0x03; // CCP1 ????????????????
    
    // CCP2 (LED)
    CCPR2L = value >> 2;
    CCP2CONbits.DC2B0 = (value & 0x01); // ???
    CCP2CONbits.DC2B1 = (value & 0x02) >> 1; //????
}

void __interrupt() ISR(void){ // ?? XC8 ??
    // ????? Timer0 ????
    if (INTCONbits.TMR0IF){
        timer_counter++;

        if (timer_counter >= TIMER_TARGET){
            timer_counter = 0; 

            if (isRunning){ 
                current += dir; //???

                // check boundary
                if (current >= POS_90){
                    current = POS_90;
                    dir = -1;
                }
                else if (current <= NEG_90){
                    current = NEG_90;
                    dir = 1;
                }
                set_motor_led(current);
            }
        }
        INTCONbits.TMR0IF = 0;
    }
}


void main(void){
    // Timer2 -> On, prescaler -> 4 
    T2CONbits.TMR2ON = 0b1;
    T2CONbits.T2CKPS = 0b01;

    // Internal Oscillator Frequency, Fosc = 125 kHz, Tosc = 8 탎
    OSCCONbits.IRCF = 0b001;
    
    // PWM mode, P1A, P1C active-high; P1B, P1D active-high
    CCP1CONbits.CCP1M = 0b1100; // CCP1 (??)
    CCP2CONbits.CCP2M = 0b1100; // CCP2 (LED)
    
    // CCP1/RC2 & CCP2/RC1 -> Output
    TRISCbits.TRISC2 = 0; // ??
    TRISCbits.TRISC1 = 0; // LED

    // RB0 -> Input (Button)
    TRISBbits.TRISB0 = 1;

    // Set up PR2 to decide PWM period
    PR2 = 0x9B; // ~= 20ms
    
    // Timer0 ?????
    // T0CON: 8-bit mode, Fosc/4, NO prescaler (PSA=1)
    // Overflow Period = 256 * (1 / (125kHz/4)) = 8.192ms
    T0CONbits.TMR0ON = 1; // Timer0 On
    T0CONbits.T08BIT = 1; // 8-bit mode
    T0CONbits.T0CS = 0;   // Clock source = Fosc/4
    T0CONbits.PSA = 1;    // Bypass prescaler
    
    INTCONbits.TMR0IE = 1; // ?? Timer0 ????
    INTCONbits.GIE = 1;    // ??????
    
    current = NEG_90;
    set_motor_led(current);
    
    char buttonWasPressed = 0; // ????????

    while(1)
    {
        // ?? "??"
        if (PORTBbits.RB0 == 0)
        {
            __delay_ms(20);
            if (PORTBbits.RB0 == 0)
            {
                buttonWasPressed = 1;
            }
        }
        // ?? "??" (???)
        else if (buttonWasPressed == 1 && PORTBbits.RB0 == 1)
        {
            __delay_ms(20);
            if (PORTBbits.RB0 == 1)
            {
                isRunning = !isRunning; // ????/????
                buttonWasPressed = 0;
            }
        }
    }
    return;
}