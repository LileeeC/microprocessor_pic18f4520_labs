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
int digits[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

void __interrupt(high_priority) H_ISR() {
    // step4: conversion completed
    if (PIR1bits.ADIF) {
        unsigned int value = ADRESH;
        
        // map 0-255 to 0-7: 256/8=32，所以每 32 個數切換一個數字
        unsigned int index = value / 16; 
//        if (index > 7) index = 7;

        // RB0-RB3
        LATD = (LATD & 0xF0); // 清空低 4 位
        LATD = LATD | (digits[index] & 0x0F);

        // clear interrupt flag bit
        PIR1bits.ADIF = 0;

        // step5: next conversion
        // delay at least 2 TAD: Fosc = 1MHz, Tad = 2us (ADCS=000) -> 2Tad = 4us.
        _delay(10); // 延遲幾個 instruction cycle
        
        ADCON0bits.GO = 1; // go back to step 3
    }
    return;
}

int ADC_Read(void) {
    ADCON0bits.GO = 1;
    while(ADCON0bits.GO);
    return (ADRESH << 8) + ADRESL;
}

void set_motor_angle(unsigned int ten_bit){
    // Set CCPRxL and CCPxCON<5:4>
    CCPR1L = ten_bit >> 2; // high 8 bits
    CCP1CONbits.DC1B = ten_bit & 0x03; // low 2 bits
}

void LED_Initialize(void){
    // Configure I/O ports
    TRISD &= 0x0F;  // Set RD4-RD7 as outputs for LED
    LATD &= 0x0F;   // Clear RD4-RD7
}

void ADC_Initialize(void){
    // Configure ADC
    TRISAbits.RA0 = 1;         // Set RA0 as input port
    ADCON0bits.CHS = 0b0000;   // Select AN0 channel
    ADCON1bits.VCFG0 = 0;      // Vref+ = Vdd
    ADCON1bits.VCFG1 = 0;      // Vref- = Vss
    ADCON2bits.ADCS = 0b000;   // ADC clock Fosc/2 Tad = 2us
    ADCON2bits.ACQT = 0b001;   // 2Tad acquisition time 4us
    ADCON0bits.ADON = 1;       // Enable ADC
    ADCON2bits.ADFM = 1;       // Right justified
    PIR1bits.ADIF = 0;         // Clear ADC flag
    IPR1bits.ADIP = 0;         // Set ADC interrupt priority to low
    PIE1bits.ADIE = 1;         // Enable ADC interrupt
}

void PWM_Initialize(void){
    // Configure servo (PWM) 
    /*
     * PWM period
     * = (PR2 + 1) * 4 * Tosc * (TMR2 prescaler)
     * = (0xFF + 1) * 4 * 1µs * 16
     * = 16.32ms
     */
    T2CONbits.TMR2ON = 0b1;      // Timer2 on
    T2CONbits.T2CKPS = 0b11;     // Prescaler 16, no need to set postscaler

    PIE1bits.TMR2IE = 0;   // disable Timer2 interrupt

    CCP1CONbits.CCP1M = 0b1100;  // PWM mode
    PR2 = 0xFF;                  // Set PWM period

    TRISCbits.TRISC2 = 0;        // CCP1 as output for servo
    LATCbits.LATC2 = 0;          // Clear CCP1
}
void main(void) {
    ADC_Initialize();
    PWM_Initialize();
    set_motor_angle(POS_0); // init
    TRISAbits.RA0 = 1;       //analog input port
    TRISB = 0x00;            // LED output
    LATB = 0x00;
    
    int vr_value = 0;
    unsigned int angle = POS_0;

    while(1) {
        // 2. 讀取 VR (0 ~ 1023)
//        vr_value = ADC_Read();

        // 3. 計算對應的 Duty Cycle (線性映射)
        // 公式：Output = Min + (Input * Range_Out / Range_In)
        // Range_Out = 150 - 93 = 57
        // Range_In  = 1023
        // 使用 long 強制轉型避免 overflow
//        angle = POS_0 + (unsigned int)((long)vr_value * (POS_POS_90 - POS_0) / 1023);

        // 4. 輸出 PWM
        set_motor_angle(angle);

        // 5. 稍微延遲 (讓 AD 轉換跟馬達有時間反應)
        __delay_ms(10);
    }
    return;
}