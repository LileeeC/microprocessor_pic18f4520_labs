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

unsigned char pre = 0;    // 上一次的 ADC 
unsigned char odd = 0; // 0: even (上升), 1: odd (下降)

void __interrupt(high_priority) H_ISR() {
    if (PIR1bits.ADIF) {
        //step4
        unsigned int cur = ADRESH;
        
        // 判斷電壓變化方向
        if (cur > pre+10) {
            odd = 0; 
            pre = cur;
        } else if (cur < pre-10) {
            odd = 1; 
            pre = cur;
        }
        // if current_val == prev_val, is_odd_mode unchanged

        // map 0-255 to 0-7
        unsigned int index = cur / 32;
        if (index > 7) index = 7;

        unsigned int num;
        if (odd == 0) {
            num = index * 2;
        } else {
            num = index * 2 + 1;
        }
        
        LATB = (LATB & 0xF0) | (num & 0x0F);
//        pre = cur;
        PIR1bits.ADIF = 0;

        // step5: delay & restart ADC
        _delay(10);
        ADCON0bits.GO = 1;
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

    //step3: start conversion
    ADCON0bits.GO = 1;

    while(1);
    return;
}