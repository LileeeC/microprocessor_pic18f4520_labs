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
#define _XTAL_FREQ 4000000 
int count = 0;
int timer = 0;
int x = 0;
int s = 0;
char t = '0';
char str[5];
int cnt = 0;

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
            TXREG = '\r'; // echoes enter
            while(!TXSTAbits.TRMT);
            TXREG = '\n'; // echoes newline
            s = 1;
            x = str[2] - 48; // 48 is '0'
            t = str[0];
            if(t == 1) timer = 0;
            else if(t == 2) timer = 1;
            else timer = 3;
            for(int i = 0; i < 3; i++) str[i] = '0';
            cnt = 0;
            if(count > timer) count = 0;
            LATA = x << 1;
        }
        else{ // not "enter"
            TXREG = data;
            str[cnt] = data;
            cnt++;
        }
    }
    PIR1bits.RCIF = 0;
    return;
}

//void __interrupt(low_priority)L_ISR(){
//    if(count == timer){
//        if(x > 0){
//            x--;
//            LATA = x << 1;
//        }
//        count = 0;
//    }
//    else count++;
//    PIR1bits.TMR2IF = 0;
//    return;
//}

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

void Timer2_Initialize(){
    // Timer2 -> On, prescaler -> 4
    //T2CONbits.TMR2ON = 0b1;
    //T2CONbits.T2CKPS = 0b11;
    PIR1bits.TMR2IF = 0;
    PIE1bits.TMR2IE = 1;
    IPR1bits.TMR2IP = 0;
    T2CON = 0xFF;
    PR2 = 0xFF;
}
void main(void) 
{
    UART_Initialize();
    OSCCONbits.IRCF = 0b110;
    //Timer2_Initialize();
    
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    RCONbits.IPEN = 1;
    
    ADCON1bits.PCFG = 0b1111;
    TRISAbits.RA1 = 0;
    TRISAbits.RA2 = 0;
    TRISAbits.RA3 = 0;
    LATA = x << 1;
    
    //T2CONbits.TMR2ON = 1;
    while(1){
        
        if(x >= 0 && s == 1){
            LATA = 1 << 1; //左移一位 (LATA = 1 << 1) $\rightarrow$ 0010 (亮 RA1)。這樣接在 RA1 的燈才會亮。
            if(t == '1'){
                LATA = x << 1;  // 動作 A: 更新 LED 顯示 x
                x--; // 動作 B: 數字減 1
                
                __delay_ms(250); // 動作 C: 等待 0.25 秒
                
            }
            else if(t == '2'){
                LATA = x << 1;// 動作 A: 更新 LED 顯示 x (跟上面一樣)
                x--;// 動作 B: 數字減 1 (跟上面一樣)
                
                __delay_ms(500); // 動作 C: 等待 0.5 秒 
                
            }
            else if(t == '3'){
                LATA = x << 1;
                x--;
                
                __delay_ms(1000);
                
            }
        }
        
    };
    
    return;
}



