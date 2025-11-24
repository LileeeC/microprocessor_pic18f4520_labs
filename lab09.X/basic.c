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

int date_digits[8] = {2, 0, 2, 5, 0, 1, 0, 1};

void __interrupt(high_priority) H_ISR() {
    // step4: conversion completed
    if (PIR1bits.ADIF) {
        unsigned int value = ADRESH;
        
        // map 0-255 to 0-7: 256/8=32，所以每 32 個數切換一個數字
        unsigned int index = value / 32; 
//        if (index > 7) index = 7;

        // RB0-RB3
        LATB = (LATB & 0xF0); // 清空低 4 位
        LATB = LATB | (date_digits[index] & 0x0F);

        // clear interrupt flag bit
        PIR1bits.ADIF = 0;

        // step5: next conversion
        // delay at least 2 TAD: Fosc = 1MHz, Tad = 2us (ADCS=000) -> 2Tad = 4us.
        _delay(10); // 延遲幾個 instruction cycle
        
        ADCON0bits.GO = 1; // go back to step 3
    }
    return;
}

void main(void) {
    //configure OSC and port
    OSCCONbits.IRCF = 0b100; //1MHz
    TRISAbits.RA0 = 1;       //analog input port
    TRISB = 0x00;            // LED output
    LATB = 0x00;            

    // step1: configure ADC module
    ADCON1bits.VCFG0 = 0;    // Vref+ = Vdd
    ADCON1bits.VCFG1 = 0;    // Vref- = Vss
    ADCON1bits.PCFG = 0b1110;// AN0 為 analog input, 其他 digital
    ADCON0bits.CHS = 0b0000; // AN0 當作 analog input
    
    // 時序設定
    // Tad = 2us (Fosc=1M, ADCS=000)
    // Tacq = 4us (ACQT=001)
    ADCON2bits.ADCS = 0b000; // 查表後設000(1Mhz < 2.86Mhz)
    ADCON2bits.ACQT = 0b001; //Tad = 2 us acquisition time設2Tad = 4 > 2.4
    ADCON0bits.ADON = 1;     // Enable ADC
    ADCON2bits.ADFM = 0;     // Left Justified since we only need 8 bits
    
    // step2: configure ADC interrupt
    PIE1bits.ADIE = 1;       // Enable ADC Interrupt
    PIR1bits.ADIF = 0;       // Clear Flag
    INTCONbits.PEIE = 1;     // Enable Peripheral Interrupts
    INTCONbits.GIE = 1;      // Enable Global Interrupts

    // step3: start conversion
    ADCON0bits.GO = 1;

    while(1) {
        // Main loop 空轉，所有工作都在 ISR 完成
    }
    
    return;
}