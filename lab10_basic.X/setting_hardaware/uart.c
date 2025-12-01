#include <xc.h>
    //setting TX/RX

char mystring[20];
int lenStr = 0;

void UART_Initialize() {
           
    /*       TODObasic   
           Serial Setting      
        1.   Setting Baud rate
        2.   choose sync/async mode 
        3.   enable Serial port (configures RX/DT and TX/CK pins as serial port pins)
        3.5  enable Tx, Rx Interrupt(optional)
        4.   Enable Tx & RX
    */       
    TRISCbits.TRISC6 = 1; // RC6 as output           
    TRISCbits.TRISC7 = 1; // RC7 as input          
    
    //  Setting baud rate
    TXSTAbits.SYNC = 0;     // asynchronous mode          
    BAUDCONbits.BRG16 = 0;  // 8-bit Baud Rate Generator         
    TXSTAbits.BRGH = 0;     // low speed
    SPBRG = 51;
    
   //   Serial enable
    RCSTAbits.SPEN = 1;     // enable Serial Port              
    PIR1bits.TXIF = 0;      // clear Transmit Interrupt Flag
    PIR1bits.RCIF = 0;      // clear Receive Interrupt Flag
    TXSTAbits.TXEN = 1;     // enable Transmission           
    RCSTAbits.CREN = 1;     // enable Reception
    
    // UART 優先權必須比 Timer 高
    PIE1bits.TXIE = 0;      // disable Tx Interrupt       
    IPR1bits.TXIP = 0;      // Tx Priority             
    PIE1bits.RCIE = 1;      // enable Rx Interrupt              
    IPR1bits.RCIP = 1;      // Rx Set to High Priority    
              
}

void UART_Write(unsigned char data){ // Output on Terminal
    while(!TXSTAbits.TRMT);
    TXREG = data;              //write to TXREG will send data 
}


void UART_Write_Text(char* text) { // Output on Terminal, limit:10 chars
    for(int i=0;text[i]!='\0';i++)
        UART_Write(text[i]);
}

void ClearBuffer(){
    for(int i = 0; i < 10 ; i++)
        mystring[i] = '\0';
    lenStr = 0;
}

void MyusartRead(){
    /* TODObasic: try to use UART_Write to finish this function */
    char data = RCREG; // read register
    UART_Write(data);
    
    // Buffer full or new line
    if (lenStr < 19) {
        mystring[lenStr++] = data;
        mystring[lenStr] = '\0';
    }
    
    return ;
}

char *GetString(){
    return mystring;
}


// void interrupt low_priority Lo_ISR(void)
//void __interrupt(low_priority)  Lo_ISR(void){
//    if(RCIF)
//    {
//        if(RCSTAbits.OERR)
//        {
//            CREN = 0;
//            Nop();
//            CREN = 1;
//        }
//        
//        MyusartRead();
//    }
//    
//   // process other interrupt sources here, if required
//    return;
//}