#include <xc.h>
#include <pic18f4520.h>
#include <stdio.h>

#pragma config OSC = INTIO67 // Oscillator Selection bits
#pragma config WDT = OFF     // Watchdog Timer Enable bit
#pragma config PWRT = OFF    // Power-up Enable bit
#pragma config BOREN = ON    // Brown-out Reset Enable bit
#pragma config PBADEN = OFF  // Watchdog Timer Enable bit
#pragma config LVP = OFF     // Low Voltage (single -supply) In-Circute Serial Pragramming Enable bit
#pragma config CPD = OFF     // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)

int date_digits[8] = {2, 0, 2, 5, 1, 1, 1, 9};

void __interrupt(high_priority) H_ISR() {
    // Step 4: Conversion completed
    if (PIR1bits.ADIF) {
        unsigned int value = ADRESH;
        
        // 將 0-255 對應到 0-7
        unsigned int index = value / 32; // 256/8=32，所以每 32 個數值切換一個數字
        
        // 防止 index 超出範圍 (雖理論上不會，但保險起見)
        if (index > 7) index = 7;

        // 3. 顯示對應數字到 LED (假設 LED 接在 PORTB 的 RB0-RB3)
        // 先清空低 4 位，再寫入新值，保留高 4 位原本狀態
        LATB = (LATB & 0xF0) | (date_digits[index] & 0x0F);

        // Clear interrupt flag
        PIR1bits.ADIF = 0;

        // Step 5: Next conversion
        // 需要等待至少 2 TAD。
        // Fosc = 1MHz, Tad = 2us (ADCS=000). 2Tad = 4us.
        _delay(10); // 延遲幾個 instruction cycle
        
        ADCON0bits.GO = 1; // 重新開始轉換
    }
    return;
}

void main(void) {
    //configure OSC and port
    OSCCONbits.IRCF = 0b100; //1MHz
    TRISAbits.RA0 = 1;       //analog input port
    TRISB = 0x00;            // PORTB as Output (LEDs)
    LATB = 0x00;             // Clear LEDs initially

    // Step 1: Configure ADC module
    ADCON1bits.VCFG0 = 0;    // Vref+ = Vdd
    ADCON1bits.VCFG1 = 0;    // Vref- = Vss
    ADCON1bits.PCFG = 0b1110;// AN0 為 analog input, 其他 digital [cite: 22]
    ADCON0bits.CHS = 0b0000; // Select Channel AN0
    
    // ADC Clock & Acquisition Time
    // Fosc = 1MHz. 為了 Tad >= 0.7us, 設 ADCS=000 (2*Tosc = 2us)
    // Acquisition time >= 2.4us. 設 ACQT=001 (2*Tad = 4us)
    ADCON2bits.ADCS = 0b000; 
    ADCON2bits.ACQT = 0b001; 
    ADCON2bits.ADFM = 0;     // Left Justified (取 ADRESH 即可得到 8-bit 解析度)
    ADCON0bits.ADON = 1;     // Enable ADC

    // Step 2: Configure ADC interrupt
    PIR1bits.ADIF = 0;       // Clear Flag
    PIE1bits.ADIE = 1;       // Enable ADC Interrupt
    INTCONbits.PEIE = 1;     // Enable Peripheral Interrupts
    INTCONbits.GIE = 1;      // Enable Global Interrupts

    // Step 3: Start conversion
    ADCON0bits.GO = 1;

    while(1) {
        // Main loop 空轉，所有工作都在 ISR 完成
    }
    
    return;
}