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

#define NEG_90 15  // -90 度 (10-bit value)
#define POS_90 80  // +90 度 (10-bit value)

// Timer0 週期: 8.192ms (1 tick = 32µs, 256 ticks overflow)
// 新的總步數 (來回): (80 - 15) * 2 = 130 步
// 目標 5 秒 (5000ms): 5000ms / 130 步 = 38.46 ms/步
// TARGET = 38.46 / 8.192 = 4.69 -> 我們使用 5
// (5 * 8.192ms = 40.96ms/步, 總時間 130 * 40.96ms = 5.3 秒, 接近 5 秒)
#define TIMER_TARGET 5

volatile char isRunning = 0; // main與ISR溝通的橋樑
volatile signed int current;
volatile signed char dir = 1;
volatile unsigned char timer_counter = 0; // 軟體計數器

void set_motor_led(unsigned int ten_bit){
    // CCP1 (motor)
    CCPR1L = ten_bit >> 2;
    CCP1CONbits.DC1B = ten_bit & 0x03; // CCP1 低位元綁在一起
    
    // CCP2 (LED)
    unsigned int led;
    // unsigned int input; // (這行在你的程式碼中沒有被使用，但保留)
    // 映射到 LED 的 0~624 範圍
    led = (ten_bit - NEG_90) * 9; // (0~65) * 9 = 0~585
    CCPR2L = ten_bit >> 2;
    CCP2CONbits.DC2B0 = (ten_bit & 0x01); // 這是 bit 4 (低位)
    CCP2CONbits.DC2B1 = (ten_bit & 0x02) >> 1; // 這是 bit 5 (高位)
}

void __interrupt() ISR(void){ // 告訴 XC8 這是中斷
    // 檢查是否為 Timer0 中斷
    if (INTCONbits.TMR0IF){ // Timer0 overflow, 硬體會將此 bit 設為 1
        timer_counter++;

        if (timer_counter >= TIMER_TARGET){
            timer_counter = 0; 

            if (isRunning){ 
                current += dir; //往前走一步

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
        INTCONbits.TMR0IF = 0; // **重要**：手動清除旗標(關掉鬧鐘)
    }
}

void main(void){
    // Timer2 -> On, prescaler -> 4 
    T2CONbits.TMR2ON = 0b1;
    T2CONbits.T2CKPS = 0b01;

    // Internal Oscillator Frequency, Fosc = 125 kHz, Tosc = 8 µs
    OSCCONbits.IRCF = 0b001;
    
    // PWM mode, P1A, P1C active-high; P1B, P1D active-high
    CCP1CONbits.CCP1M = 0b1100; // CCP1 (馬達)
    CCP2CONbits.CCP2M = 0b1100; // CCP2 (LED)
    
    // CCP1/RC2 & CCP2/RC1 -> Output
    TRISCbits.TRISC2 = 0; // 馬達
    TRISCbits.TRISC1 = 0; // LED

    // RB0 -> Input (Button)
    TRISBbits.TRISB0 = 1;

    // Set up PR2 to decide PWM period
    PR2 = 0x9B; // ~= 20ms
    
    // timer setting
    T0CONbits.TMR0ON = 1; // Timer0 On
    T0CONbits.T08BIT = 1; // 8-bit timer
    T0CONbits.T0CS = 0;   // Clock source = Fosc/4
    T0CONbits.PSA = 1;    // 預分頻器不分配給 Timer0
    
    // interrupt setting
    INTCONbits.TMR0IE = 1; // 允許 Timer0 溢位中斷
    INTCONbits.GIE = 1;    // 啟用全域中斷
    
    current = NEG_90;
    set_motor_led(current);
    char pressed = 0;

    while(1){
        if (PORTBbits.RB0 == 0){ // pressed
            __delay_ms(20);
            if (PORTBbits.RB0 == 0){
                pressed = 1;
            }
        }
        else if (pressed == 1 && PORTBbits.RB0 == 1){ // unpressed
            __delay_ms(20);
            if (PORTBbits.RB0 == 1){
                isRunning = !isRunning; 
                pressed = 0;
            }
        }
    }
    return;
}