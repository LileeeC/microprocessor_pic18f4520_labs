#include <xc.h>
#include<stdio.h>
#include<stdlib.h>
#include <time.h>

#pragma config OSC = INTIO67  //OSCILLATOR SELECTION BITS (INTERNAL OSCILLATOR BLOCK, PORT FUNCTION ON RA6 AND RA7)
#pragma config WDT = OFF      //Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config PWRT = OFF     //Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON     //Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config PBADEN = OFF   //PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
#pragma config LVP = OFF      //Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config CPD = OFF      //Data EEPROM Code Protection bit (Data EEPROM not code-protected)
#define _XTAL_FREQ 4000000 
int mode=0,timertime=0;
void __interrupt(high_priority)H_ISR(){
    
    //step4
    if(ADRES<=350)
    {mode=1;}
    else if(ADRES<=700)
    {mode=2;}
    else
    {mode=3;}
    //do things
   
    //clear flag bit
    PIR1bits.ADIF = 0;
    
    int times;
if(mode==1){times=8;}
else if(mode==2){times=4;}
else {times=2;}
    if(PIR1bits.TMR2IF==1){
timertime++;
if(timertime>=times)
{LATB^=2;
timertime=0;}}
PIR1bits.TMR2IF=0;
    //step5 & go back step3
    /*
    delay at least 2tad
    ADCON0bits.GO = 1;
    */
    
    return;
}

void main(void) 
{
    TRISB = 0x00;
    OSCCONbits.IRCF = 0b110; //4MHz
    TRISAbits.RA0 = 1;       //analog input port
    
    //step1
    ADCON1bits.VCFG0 = 0;
    ADCON1bits.VCFG1 = 0;
    ADCON1bits.PCFG = 0b1110; //AN0 ?analog input,???? digital
    ADCON0bits.CHS = 0b0000;  //AN0 ?? analog input
    ADCON2bits.ADCS = 0b100;  //????000(1Mhz < 2.86Mhz)
    ADCON2bits.ACQT = 0b010;  //Tad = 1 us acquisition time  4Tad = 4 > 2.4
    ADCON0bits.ADON = 1;
    ADCON2bits.ADFM = 1;    //riht justified 
    
    
    //step2
    PIE1bits.ADIE = 1;
    PIR1bits.ADIF = 0;
    RCONbits.IPEN=0;
    INTCONbits.INT0IF=0;
     INTCONbits.INT0IE=1;
      INTCONbits.GIEH=1;
      INTCONbits.GIEL=1;
      INTCONbits.GIE=1;
      PIR1bits.TMR2IF=0;
      PIE1bits.TMR2IE=1;
      IPR1bits.TMR2IP=1;
     TRISA=1;
     LATB=2;
     T2CON=255;
     PR2=244;


    //step3
    ADCON0bits.GO = 1;
    
    while(1){
        __delay_ms(20);
        ADCON0bits.GO = 1;
    }
    
    return;
}
