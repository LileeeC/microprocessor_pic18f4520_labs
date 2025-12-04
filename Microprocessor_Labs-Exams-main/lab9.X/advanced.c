#include <xc.h>
#include <pic18f4520.h>
#include <stdio.h>

#pragma config OSC = INTIO67 // Oscillator Selection bits
#pragma config WDT = OFF     // Watchdog Timer Enable bit
#pragma config PWRT = OFF    // Power-up Enable bit
#pragma config BOREN = ON    // Brown-out Reset Enable bit
#pragma config PBADEN = OFF  // Watchdog Timer Enable bit
#pragma config LVP = OFF     // Low Voltage (single -supply) In-Circute Serial Pragramming Enable bit
#pragma config CPD = OFF     // Data EEPROM?Memory Code Protection bit (Data EEPROM code protection off)

#define _XTAL_FREQ 1000000 // ???? 8MHz ???

int value = 0;
void __interrupt(high_priority)H_ISR(){

    //step4
    
    value = (ADRESH << 2) | (ADRESL >> 6);

    //clear flag bit
    PIR1bits.ADIF = 0;
    
    return;
}

void __interrupt(low_priority)L_ISR(){
    return;
}
int find_pattern(int value, int direction){
    if(direction == 1){
        if(value < 128){
            return 0;
        }
        else if(value < 256){
            return 2;
        }
        else if(value < 384){
            return 4;
        }
        else if(value < 512){
            return 6;
        }
        else if(value < 640){
            return 8;
        }
        else if(value < 768){
            return 10;
        }
        else if(value < 896){
            return 12;
        }
        else{
            return 14;
        }
    }
    else if(direction == 0){
        if(value < 128){
            return 1;
        }
        else if(value < 256){
            return 3;
        }
        else if(value < 384){
            return 5;
        }
        else if(value < 512){
            return 7;
        }
        else if(value < 640){
            return 9;
        }
        else if(value < 768){
            return 11;
        }
        else if(value < 896){
            return 13;
        }
        else{
            return 15;
        }
    }
    
    return 0;

}
void main(void) 
{
    //configure OSC and port
    OSCCONbits.IRCF = 0b100; //1MHz
    TRISAbits.RA0 = 1;       //analog input port
    
    //step1
    ADCON1bits.VCFG0 = 0;
    ADCON1bits.VCFG1 = 0;
    ADCON1bits.PCFG = 0b1110; //AN0 ?analog input,???? digital
    ADCON0bits.CHS = 0b0000;  //AN0 ?? analog input
    ADCON2bits.ADCS = 0b000;  //????000(1Mhz < 2.86Mhz)
    ADCON2bits.ACQT = 0b001;  //Tad = 2 us acquisition time?2Tad = 4 > 2.4
    ADCON0bits.ADON = 1;
    ADCON2bits.ADFM = 0;    //left justified 
    
    
    //step2
    PIE1bits.ADIE = 1;
    PIR1bits.ADIF = 0;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    
    //light bulbs
    LATD = 0x00; 
    PORTD = 0x00; 
    TRISD = 0x00;

    //step3
    ADCON0bits.GO = 1;
    
    int direction = 1;
    int old_val = 0;
    
    while(1){
        LATD = find_pattern(value, direction);
        if(old_val + 20 < value){
            old_val = value;
            direction = 1;
        }
        else if(old_val - 20 > value){
            old_val = value;
            direction = 0;
        }
        __delay_us(4);
        ADCON0bits.GO = 1;
    };
    
    
    return;
}
