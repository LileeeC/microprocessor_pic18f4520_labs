#include <xc.h>
#include <pic18f4520.h>
#include <stdio.h>
#include <string.h>

#pragma config OSC = INTIO67 // Oscillator Selection bits
#pragma config WDT = OFF     // Watchdog Timer Enable bit
#pragma config PWRT = OFF    // Power-up Enable bit
#pragma config BOREN = ON    // Brown-out Reset Enable bit
#pragma config PBADEN = OFF  // Watchdog Timer Enable bit
#pragma config LVP = OFF     // Low Voltage (single -supply) In-Circute Serial Pragramming Enable bit
#pragma config CPD = OFF     // Data EEPROM?Memory Code Protection bit (Data EEPROM code protection off)
#define _XTAL_FREQ 8000000 

long long int curr = 0;
char str[10];
long long int last = 0;
char buf;
void UART_Initialize() {
           
    /*       TODObasic   
           Serial Setting      
        1.   Setting Baud rate
        2.   choose sync/async mode 
        3.   enable Serial port (configures RX/DT and TX/CK pins as serial port pins)
        3.5  enable Tx, Rx Interrupt(optional)
        4.   Enable Tx & RX
    */  
    TRISCbits.TRISC6 = 1;            
    TRISCbits.TRISC7 = 1;            
    
    //  Setting baud rate
    TXSTAbits.SYNC = 0;           
    BAUDCONbits.BRG16 = 0;          
    TXSTAbits.BRGH = 0;
    SPBRG = 51;      
    
   //   Serial enable
    RCSTAbits.SPEN = 1;              
    PIR1bits.TXIF = 1;
    PIR1bits.RCIF = 0;
    TXSTAbits.TXEN = 1;           
    RCSTAbits.CREN = 1;             
    PIE1bits.TXIE = 0;       
    IPR1bits.TXIP = 0;             
    PIE1bits.RCIE = 1;              
    IPR1bits.RCIP = 1;           
}

void UART_Write(unsigned char data)  // Output on Terminal
{
    while(!TXSTAbits.TRMT);
    TXREG = data;              //write to TXREG will send data 
}

void UART_Write_Text(char* text) { // Output on Terminal, limit:10 chars
    for(int i=0;text[i]!='\0';i++)
        UART_Write(text[i]);
}

void State(long long int x){
    x %= 7;
    if(last != x){
        if(x == 1){
            LATA = 0b00001110;
            strcpy(str, "LLL\r\n");
        }
        else if(x == 2){
            LATA = 0b00000110;
            strcpy(str, "DLL\r\n");
        }
        else if(x == 3){
            LATA = 0b00000010;
            strcpy(str, "DDL\r\n");
        }
        else if(x == 4){
            LATA = 0b00000000;
            strcpy(str, "DDD\r\n");
        }
        else if(x == 5){
            LATA = 0b00001000;
            strcpy(str, "LDD\r\n");
        }
        else if(x == 6){
            LATA = 0b00001100;
            strcpy(str, "LLD\r\n");
        }
        else if(x == 0){
            LATA = 0b00001110;
            strcpy(str, "LLL\r\n");
        }
        UART_Write_Text(str);
        last = x;
    }
    __delay_us(100);
}

void __interrupt(high_priority)H_ISR(){
    if(PIR1bits.RCIF)
    {
        if(RCSTAbits.OERR)
        {
            CREN = 0;
            Nop();
            CREN = 1;
        }
        
        char data = RCREG;
        if(data == '\r'){
            while(!TXSTAbits.TRMT);
            TXREG = '\r';
            while(!TXSTAbits.TRMT);
            TXREG = '\n';
            curr = (buf - 48);
        }
        else{
            TXREG = data;
            buf = data;
        }
    }
    PIR1bits.RCIF = 0;
    return;
}

void __interrupt(low_priority)L_ISR(){
    
    //step4
    int value = ADRESH;
    
    
    //do things
    if(value < 5) {
        State(curr + 1);
    }
    else if(value <= 42*1) {
        State(curr + 2);
    }
    else if(value <= 42*1+10 && value <= 42*2) {
        State(curr + 3);
    }
    else if(value <= 42*2+10 && value <= 42*3) {
        State(curr + 4);
    }
    else if(value <= 42*3+10 && value <= 42*4) {
        State(curr + 5);
    }
    else if(value <= 42*4+5 && value <= 42*5) {
        State(curr + 6);
    }
    else if(value <= 42*5+5)  {
        State(curr + 7);
    }
    
    
    //clear flag bit
    PIR1bits.ADIF = 0;
    
    
    //step5 & go back step3
    /*
    delay at least 2tad
    ADCON0bits.GO = 1;
    */
    __delay_us(100);
    ADCON0bits.GO = 1;
    
    return;
}

void main(void) 
{
    UART_Initialize();
    //configure OSC and port
    OSCCONbits.IRCF = 0b110; //1MHz
    TRISAbits.RA0 = 1;       //analog input port
    TRISAbits.RA1 = 0;
    TRISAbits.RA2 = 0;
    TRISAbits.RA3 = 0;
    TRISAbits.RA4 = 0;
    
    //step1
    ADCON1bits.VCFG0 = 0;
    ADCON1bits.VCFG1 = 0;
    ADCON1bits.PCFG = 0b1110; //AN0 ?analog input,???? digital
    ADCON0bits.CHS = 0b0000;  //AN0 ?? analog input
    ADCON2bits.ADCS = 0b000;  //????000(1Mhz < 2.86Mhz)
    ADCON2bits.ACQT = 0b010;  //Tad = 2 us acquisition time?2Tad = 4 > 2.4
    ADCON0bits.ADON = 1;
    ADCON2bits.ADFM = 0;    //left justified 
    
    
    //step2
    PIE1bits.ADIE = 1;
    PIR1bits.ADIF = 0;
    IPR1bits.ADIP = 0;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    RCONbits.IPEN = 1;


    //step3
    ADCON0bits.GO = 1;
    
    while(1);
    
    return;
}



