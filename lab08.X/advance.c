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

// ???: (63 - 31) = 32 steps
// 135 ? = (135 / 180) * 32 steps = 24 steps
#define STEP_135 24

void set_motor_angle(unsigned int ten_bit_value){
    // Set CCPRxL and CCPxCON<5:4>
    CCPR1L = ten_bit_value >> 2; // high 8 bits
    CCP1CONbits.DC1B = ten_bit_value & 0x03; // low 2 bits
}

void move_gradually(unsigned int start, unsigned int end){
    if (end > start){ // ??
        for (unsigned int i = start; i <= end; i++){
            set_motor_angle(i);
            __delay_ms(20); // ??????
        }
    }
    else{ // ??
        for (unsigned int i = start; i >= end; i--){
            set_motor_angle(i);
            __delay_ms(20); // ??????
        }
    }
}

void main(void){
    // Timer2 -> On, prescaler -> 4
    T2CONbits.TMR2ON = 0b1;
    T2CONbits.T2CKPS = 0b01;

    // Internal Oscillator Frequency, Fosc = 125 kHz, Tosc = 8 µs
    OSCCONbits.IRCF = 0b001;
    
    // PWM mode, P1A, P1C active-high; P1B, P1D active-high
    CCP1CONbits.CCP1M = 0b1100;
    
    // CCP1/RC2 -> Output
    TRISCbits.TRISC2 = 0;

    // RB0 -> Input (Button)
    TRISBbits.TRISB0 = 1;

    // Set up PR2 to decide PWM period 
    PR2 = 0x9B; // ~= 20ms
    
    signed int current;
    signed int target;
    signed char dir = 1; // 1 = ?+90, -1 = ?-90
    char pressed = 0;
    set_motor_angle(NEG_90);

    while(1){
        if (PORTBbits.RB0 == 0){ // pressed
            __delay_ms(20);
            if (PORTBbits.RB0 == 0){
                pressed = 1;
            }
        }else if (pressed == 1 && PORTBbits.RB0 == 1){ // unpressed
            __delay_ms(20);
            if (PORTBbits.RB0 == 1){
                target = current + (dir * STEP_135);

                // check boundary
                if (target > POS_90){ // ?? +90 
                    signed int toomuch = target - POS_90;
                    signed int final = POS_90 - toomuch;
                    
                    move_gradually(current, POS_90); // touch boundary
                    move_gradually(POS_90, final); // rotate back

                    current = final;
                    dir = -1;
                }else if (target < NEG_90){ // ?? -90
                    signed int toomuch = NEG_90 - target;
                    signed int final = NEG_90 + toomuch;

                    move_gradually(current, NEG_90); // touch boundary
                    move_gradually(NEG_90, final); // rotate back
                    
                    current = final;
                    dir = 1;
                }else{
                    move_gradually(current, target);
                    current = target;
                }
                pressed = 0;
            }
        }
    }
    return;
}