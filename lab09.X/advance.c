#include <xc.h>
#include <pic18f4520.h>
#include <stdlib.h> // 用於 abs 函數 (選用)

#pragma config OSC = INTIO67 // Oscillator Selection bits
#pragma config WDT = OFF     // Watchdog Timer Enable bit
#pragma config PWRT = OFF    // Power-up Enable bit
#pragma config BOREN = ON    // Brown-out Reset Enable bit
#pragma config PBADEN = OFF  // Watchdog Timer Enable bit
#pragma config LVP = OFF     // Low Voltage (single -supply) In-Circute Serial Pragramming Enable bit
#pragma config CPD = OFF     // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)

unsigned char prev_val = 0;    // 上一次的 ADC 
unsigned char odd_mode = 0; // 0: even (上升), 1: odd (下降)

void __interrupt(high_priority) H_ISR() {
    if (PIR1bits.ADIF) {
        //step4
        unsigned int current_val = ADRESH;
        
        // 判斷電壓變化方向
        // 為了避免雜訊造成 LED 頻繁在奇偶間閃爍，可以加一點緩衝(Threshold)，但這裡依據題目基本要求，直接比較大小即可。
        if (current_val > prev_val) {
            odd_mode = 0; 
        } else if (current_val < prev_val) {
            odd_mode = 1; 
        }
        // if current_val == prev_val, is_odd_mode unchanged

        // 將 0-255 對應到 0-7
        unsigned int index = current_val / 32;
        if (index > 7) index = 7;

        unsigned int display_value;
        
        if (odd_mode == 0) {
            display_value = index * 2;
        } else {
            display_value = index * 2 + 1;
        }

        // 輸出到 LED (保留 PORTB 高 4 位，寫入低 4 位)
        LATB = (LATB & 0xF0) | (display_value & 0x0F);

        prev_val = current_val;

        PIR1bits.ADIF = 0;

        // Delay & Restart ADC
        _delay(10);
        ADCON0bits.GO = 1;
    }
    return;
}

void main(void) {
    //configure OSC and port
    OSCCONbits.IRCF = 0b100; // 1 MHz
    TRISAbits.RA0 = 1;       // AN0 輸入
    TRISB = 0x00;            // LED 輸出
    LATB = 0x00;

    // ADC 設定 (Step 1)
    ADCON1bits.VCFG0 = 0;    // Vref+ = Vdd
    ADCON1bits.VCFG1 = 0;    // Vref- = Vss
    ADCON1bits.PCFG = 0b1110;// AN0 Analog
    ADCON0bits.CHS = 0b0000; // Channel 0
    
    // 時序設定
    // Tad = 2us (Fosc=1M, ADCS=000)
    // Tacq = 4us (ACQT=001)
    ADCON2bits.ADCS = 0b000; 
    ADCON2bits.ACQT = 0b001; 
    ADCON2bits.ADFM = 0;     // Left Justified
    ADCON0bits.ADON = 1;

    // 中斷設定 (Step 2)
    PIR1bits.ADIF = 0;
    PIE1bits.ADIE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;

    // 開始轉換 (Step 3)
    ADCON0bits.GO = 1;

    while(1);
    return;
}