#include <xc.h>
#include <pic18f4520.h>

#pragma config OSC = INTIO67 // Oscillator Selection bits
#pragma config WDT = OFF     // Watchdog Timer Enable bit
#pragma config PWRT = OFF    // Power-up Enable bit
#pragma config BOREN = ON    // Brown-out Reset Enable bit
#pragma config PBADEN = OFF  // Watchdog Timer Enable bit
#pragma config LVP = OFF     // Low Voltage (single -supply) In-Circute Serial Pragramming Enable bit
#pragma config CPD = OFF     // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)

void __interrupt(high_priority) H_ISR() {
    if (PIR1bits.ADIF) {
        //step4
        unsigned int adc_value = ADRESH;
        
        // 設定 PWM Duty Cycle (亮度)
        // 順時針(電壓增加) -> 變暗
        // ADC 越大，Duty Cycle 越小
        CCPR1L = 255 - adc_value;
        
        // 清除中斷旗標
        PIR1bits.ADIF = 0;
        
        // Delay & Restart ADC
        _delay(10);
        ADCON0bits.GO = 1;
    }
    return;
}

void main(void) {
    //configure OSC and port
    OSCCONbits.IRCF = 0b100; //1MHz
    TRISAbits.RA0 = 1;       //analog input port
    TRISCbits.TRISC2 = 0;    // RC2/CCP1 Output (LED 接這裡!)
    
    //step1
    ADCON1bits.VCFG0 = 0;    // Vref+ = Vdd
    ADCON1bits.VCFG1 = 0;    // Vref- = Vss
    ADCON1bits.PCFG = 0b1110;// AN0 Analog
    ADCON0bits.CHS = 0b0000; // Channel 0
    
    ADCON2bits.ADCS = 0b000; // Tad = 2us
    ADCON2bits.ACQT = 0b001; // Tacq = 4us
    ADCON2bits.ADFM = 0;     // Left Justified (方便直接對應 8-bit PWM)
    ADCON0bits.ADON = 1;

    // PWM 模組設定
    // 設定 Timer 2
    PR2 = 255;               // PWM Period 設定為最大解析度 (配合 ADRESH 0-255)
    T2CON = 0b00000100;      // Timer2 On, Prescaler = 1
    
    // 設定 CCP1
    CCP1CONbits.CCP1M = 0b1100; // 設定為 PWM mode
    
    //step2
    PIR1bits.ADIF = 0;
    PIE1bits.ADIE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;

    //step3
    ADCON0bits.GO = 1;

    while(1);
    return;
}