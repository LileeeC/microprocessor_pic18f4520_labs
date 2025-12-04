// =========================
// PIC18F4520 - XC8 C ??
// =========================
#include <xc.h>
#include <pic18f4520.h>

// -------------------------
// Configuration Bits (???? .inc ??)
// -------------------------
#pragma config OSC   = INTIO67     // Internal oscillator, RA6/RA7 ? I/O
#pragma config FCMEN = OFF
#pragma config IESO  = OFF

#pragma config PWRT  = OFF
#pragma config BOREN = SBORDIS
#pragma config BORV  = 3

#pragma config WDT   = OFF
#pragma config WDTPS = 32768

#pragma config CCP2MX = PORTC
#pragma config PBADEN = ON         // ??? RB0~RB4 ???????? ADCON1 ????
#pragma config LPT1OSC = OFF
#pragma config MCLRE  = ON

#pragma config STVREN = ON
#pragma config LVP    = OFF
#pragma config XINST  = OFF

#pragma config CP0 = OFF, CP1 = OFF, CP2 = OFF, CP3 = OFF
#pragma config CPB = OFF, CPD = OFF
#pragma config WRT0 = OFF, WRT1 = OFF, WRT2 = OFF, WRT3 = OFF
#pragma config WRTC = OFF, WRTB = OFF, WRTD = OFF
#pragma config EBTR0 = OFF, EBTR1 = OFF, EBTR2 = OFF, EBTR3 = OFF
#pragma config EBTRB = OFF

// ??? __delay_ms ???????? Fosc??????????????????
#define _XTAL_FREQ 250000UL   // ??????? 250 kHz

int running = 0; 
int duty_cycle_bits = 0b0000010000;
int direction = 1;
// -------------------------
// ?????????? (?? org 0x08 ? ISR)
// ???????? LATA?? TMR2IF
// -------------------------
void __interrupt(high_priority) HighISR(void)
{   
    if(INTCONbits.INT0IF){
        INTCONbits.INT0IE = 0;          // ???? INT0?????????
        running = !running; 
        __delay_ms(20);     // ?? 0.5 ??????

        INTCONbits.INT0IF = 0;          // ??????
            INTCONbits.INT0IE = 1;          // ???? INT0

    }
    if(PIR1bits.TMR1IF){
        
        if(running == 1){
            if(direction == 1){
                duty_cycle_bits += 3;
                if(duty_cycle_bits >= 79){
                    direction = 0;
                }
            }
            else{
                duty_cycle_bits -= 3;
                if(duty_cycle_bits <= 16){
                    direction = 1;
                }
            }
        }
        TMR1 = 61629;
        PIR1bits.TMR1IF = 0;
    }
}

// -------------------------
// ???????? Initial: ???
// -------------------------
static void init(void)
{
    // 1) ? PORTA ???? I/O
    ADCON1 = 0x0F;     // ???????

    // 3) ??????????????????
    RCONbits.IPEN  = 1;   // ???????
    INTCONbits.GIEH = 1;  // ????????
    // ???????????? INTCONbits.GIEL?
    //setup rb0
    
    TRISBbits.RB0 = 1; //set RB0 as input
    INTCONbits.INT0IF = 0; //clear interrupt0 flag
    INTCONbits.INT0IE = 1; //enable interrupt0
    INTCON2bits.INTEDG0 = 1; // rising edge triggered (released)*/

    // 4) ?? Timer1 ??????????
    PIR1bits.TMR1IF = 0;
    IPR1bits.TMR1IP = 1;
    PIE1bits.TMR1IE = 1;
    
    T1CONbits.T1CKPS = 0b00;
    T1CONbits.TMR1CS = 0b0;
    
    TMR1 = 61629;
    
    
    // Timer2 -> On, prescaler -> 4
    T2CONbits.TMR2ON = 0b1;
    T2CONbits.T2CKPS = 0b01;

    // Internal Oscillator Frequency, Fosc = 125 kHz, Tosc = 8 �s
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
     * = (0x9B + 1) * 4 * 8�s * 4
     * = 0.019968s ~= 20ms
     */
    PR2 = 0x9B;

    CCPR1L = 0x04;
    CCP1CONbits.DC1B = 0b00; //-90

    __delay_ms(300);
    T1CONbits.TMR1ON = 0b1; 
   
}

// -------------------------
// ???????????? main: bra main?
// -------------------------
void main(void)
{
    init();

    // ?????LATA ?? ~0.5 s ????????
    while (1) {
        CCPR1L = duty_cycle_bits >> 2;
        CCP1CONbits.DC1B = duty_cycle_bits & 0b11;
        NOP();
    }
}
