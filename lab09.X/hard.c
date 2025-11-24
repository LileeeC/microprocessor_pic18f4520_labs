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
        unsigned int adc= ADRESH;
        
        // 順時針(電壓增加)要變暗 -> ADC 越大，Duty Cycle(brightness) 越小
        CCPR1L = 255 - adc; // both ADRESH and CCPR1L are 0~255 
        
        PIR1bits.ADIF = 0;
        _delay(10);
        ADCON0bits.GO = 1;
    }
    return;
}

void main(void) {
    //configure OSC and port
    OSCCONbits.IRCF = 0b100; //1MHz
    TRISAbits.RA0 = 1;       //analog input port
    TRISCbits.TRISC2 = 0;            // LED output

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
    
    // PWM module & Timer2
    PR2 = 255;               // PWM Period 配合 ADRESH 0-255
    T2CON = 0b00000100;      // Timer2 On, Prescaler = 1
    CCP1CONbits.CCP1M = 0b1100; // PWM mode -> LED output at RC2
    
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