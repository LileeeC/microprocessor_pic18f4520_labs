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

#define _XTAL_FREQ 125000 // 晶片時脈 Fosc (Hz), for __delay_ms()
// 編譯器會「在編譯的當下」根據你必須提供的 _XTAL_FREQ，自動計算出要延遲 20 毫秒到底需要多少個 NOP 或其他迴圈

// 10-bit PWM Duty cycle time resolution = (10-bit value) * Tosc * (TMR2 Prescaler)
// 1 tick = Tosc * (TMR2 Prescaler)
// Fosc = 125kHz, Tosc = 8µs, TMR2 Prescaler = 4 -> 1 tick = 32µs
#define POS_NEG_90 15  // -90 度 
#define POS_NEG_45 30  // -45 度 
#define POS_0 45       // 0 度  
#define POS_POS_45 63  // +45 度 
#define POS_POS_90 80  // +90 度 
int count = 0;

void LED_Initialize(void){
    // Configure I/O ports
    TRISD &= 0x0F;  // Set RD4-RD7 as outputs for LED
    LATD &= 0x0F;   // Clear RD4-RD7
}

void set_motor_angle(unsigned int ten_bit){
    // Set CCPRxL and CCPxCON<5:4>
    CCPR1L = ten_bit >> 2; // high 8 bits
    CCP1CONbits.DC1B = ten_bit & 0x03; // low 2 bits
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
    /*
     * PWM period
     * = (PR2 + 1) * 4 * Tosc * (TMR2 prescaler)
     * = (0x9B + 1) * 4 * 8µs * 4
     * = 0.019968s ~= 20ms
     */
    PR2 = 0x9B;
    set_motor_angle(POS_0); // init

    static unsigned char motorState = 0; 
    static char pressed = 0; 

    while(1){
        if (PORTBbits.RB0 == 0){ // button pressed
            __delay_ms(20); // bounce delay
            if (PORTBbits.RB0 == 0){
                pressed = 1;
            }
        }else if (pressed == 1 && PORTBbits.RB0 == 1){ // marked && unpressed
            __delay_ms(20);
            if (PORTBbits.RB0 == 1){
                motorState = (motorState + 1) % 8; // state change in cycle
                count++;
                if(motorState == 0){
                    set_motor_angle(POS_0); 
                    LATD = (count % 16) << 4;
                }else if(motorState == 1){
                    set_motor_angle(POS_POS_45); 
                    LATD = (count % 16) << 4;
                }else if(motorState == 2){
                    set_motor_angle(POS_POS_90); 
                    LATD = (count % 16) << 4;
                }else if(motorState == 3){
                    set_motor_angle(POS_POS_45); 
                    LATD = (count % 16) << 4;
                }else if(motorState == 4){
                    set_motor_angle(POS_0); 
                    LATD = (count % 16) << 4;
                }else if(motorState == 5){
                    set_motor_angle(POS_NEG_45); 
                    LATD = (count % 16) << 4;
                }else if(motorState == 6){
                    set_motor_angle(POS_NEG_90); 
                    LATD = (count % 16) << 4;
                }else if(motorState == 7){
                    set_motor_angle(POS_NEG_45); 
                    LATD = (count % 16) << 4;
                }
                
                pressed = 0;
            }
        }
    }
    return;
}