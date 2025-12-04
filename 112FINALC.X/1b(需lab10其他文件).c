#include "setting.h"
#include <stdlib.h>
#include "stdio.h"
#include "string.h"
#define _XTAL_FREQ 1000000
// using namespace std;
int mode=0,timertime=0,count=0;
char str[20];
void Mode1(){   // Todo : Mode1 
    return ;
}
void Mode2(){   // Todo : Mode2 
    return ;
}
void main(void) 
{
    SYSTEM_Initialize();
 
    
   
    
    
       
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
     LATA=2;
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

void __interrupt(high_priority) Hi_ISR(void)
{
 int value = ADRES/400;
    mode=value+1;
    
    //do things
   
    //clear flag bit
    PIR1bits.ADIF = 0;
    
    int times;
if(mode==1){times=8;}
else if(mode==2){times=4;}
else {times=2;}
    if(PIR1bits.TMR2IF==1){
timertime++;

unsigned char c,b;


if(timertime>=times)
{LATA^=2;
count++;
timertime=0;
UART_Write('S');
UART_Write('t');
UART_Write('a');
UART_Write('t');
UART_Write('e');
UART_Write('_');
if(mode==1){UART_Write('1');}
else if(mode==2){UART_Write('2');}
else {UART_Write('3');}
UART_Write(' ');
UART_Write('c');
UART_Write('o');
UART_Write('u');
UART_Write('n');
UART_Write('t');
UART_Write('=');
switch(count/10)
{
    case 0:
        
        break;
        case 1:
           UART_Write('1');
        break;
        case 2:
         UART_Write('2');
        break;
        case 3:
           UART_Write('3');
        break;
        case 4:
           UART_Write('4');
        break;
        case 5:
         UART_Write('5');
        break;
        case 6:
           UART_Write('6');
        break;
        case 7:
           UART_Write('7');
        break;
        case 8:
           UART_Write('8');
        break;
        case 9:
           UART_Write('9');
        break;
}
switch(count%10)
{
    case 0:
        UART_Write('0');
        break;
        case 1:
           UART_Write('1');
        break;
        case 2:
         UART_Write('2');
        break;
        case 3:
           UART_Write('3');
        break;
        case 4:
           UART_Write('4');
        break;
        case 5:
         UART_Write('5');
        break;
        case 6:
           UART_Write('6');
        break;
        case 7:
           UART_Write('7');
        break;
        case 8:
           UART_Write('8');
        break;
        case 9:
           UART_Write('9');
        break;
}
UART_Write('\r');
UART_Write('\n');}}
PIR1bits.TMR2IF=0;
return;
}