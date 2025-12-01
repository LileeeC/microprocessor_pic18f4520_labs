#include "setting_hardaware/setting.h"
#include <stdlib.h>
#include "stdio.h"
#include "string.h"
// using namespace std;

#define _XTAL_FREQ 4000000

extern int lenStr;
extern void MyusartRead();
extern void ClearBuffer();
extern char *GetString();

int current_mode = 0; // 0:Menu, 1:Basic, 2:Advance
int counter = 0;
int old_val = 0;
int tick_count = 0;
int delay_target = 20; // 預設 1.0秒 (20 * 50ms)
char str[20];

void TMR2_Initialize(void) {
    // 50ms 中斷一次 (Fosc=4M)
    T2CON = 0x7E; // 1:16 prescaler, 1:16 postscaler
    PR2 = 195; 
    PIR1bits.TMR2IF = 0; // clear interrupt flag
    IPR1bits.TMR2IP = 0; // Low Priority
    PIE1bits.TMR2IE = 1; // Enable Timer2
}

void Mode1(){
    UART_Write_Text("\r\nMode 1 (Button)\r\n");
    
    TRISBbits.TRISB0 = 1; // Button Input
    TRISD = 0x00; LATD = 0x00; // LED Output
    PIE1bits.TMR2IE = 0; // 關閉 Timer2
    
    counter = 0;
    old_val = 0;
    UART_Write_Text("0");

    while(1){
        // 偵測按鈕
        if(PORTBbits.RB0 == 0) {
            __delay_ms(20);
            if(PORTBbits.RB0 == 0) {
                while(PORTBbits.RB0 == 0); // button released
                
                counter++;
                LATD = (counter % 16) << 4; // RD4-RD7

                // UART In-Place Update
                if (old_val < 10){
                    UART_Write('\b');
                }
                else if(old_val < 100) { 
                    UART_Write('\b'); 
                    UART_Write('\b'); 
                }
                else { 
                    UART_Write('\b'); 
                    UART_Write('\b'); 
                    UART_Write('\b'); 
                }
                
                // show
                char buffer[10];
                sprintf(buffer, "%d", counter);
                UART_Write_Text(buffer);
                
                old_val = counter;
            }
        }
    }
}

void Mode2(){
    UART_Write_Text("\r\nMode 2 (Timer)\r\n");
    
    ClearBuffer();
    TRISD = 0x00; 
    LATD = 0x00; // LED Output
    counter = 0;
    TMR2_Initialize();
    current_mode = 2; 

    while(1){
        // 計數在 Low_ISR ，字串處理在 High_ISR 
    }
}

void main(void) {
    SYSTEM_Initialize() ;
    while(1) {
        strcpy(str, GetString()); // TODO : GetString() in uart.c
        if(str[0]=='m' && str[1]=='1'){ // Mode1
            Mode1();
            ClearBuffer();
        }
        else if(str[0]=='m' && str[1]=='2'){ // Mode2
            Mode2();
            ClearBuffer();  
        }
    }
    while(1);
    return;
}

// High Priority Interrupt for keyboard input
void __interrupt(high_priority) Hi_ISR(void){
    if(PIR1bits.RCIF) {
        // 確認緩衝區有沒有滿
        if(RCSTAbits.OERR) {
            RCSTAbits.CREN = 0; 
            Nop(); 
            RCSTAbits.CREN = 1;
        }
        MyusartRead(); // echo
        
        // mode 2 speed adjustment
        if(current_mode == 2) {
            char *str = GetString();
            if(lenStr > 0 && str[lenStr-1] == '\r') {
                int new_speed = 0;
                if(str[0] == '1' && str[1] == '.'){
                    new_speed = 10;
                    
                }
                else if (str[0] == '0' && str[1] == '.'){
                    new_speed = str[2] - '0';
                }

                if(new_speed >= 1 && new_speed <= 10) {
                   delay_target = new_speed * 2;
                   if(tick_count >= delay_target){
                       tick_count = delay_target;
                   }
                }
                
                UART_Write('\r');
                UART_Write('\n');
                ClearBuffer();
            }
        }
    }
}

// low priority for Timer2
void __interrupt(low_priority) Lo_ISR(void){
    if(PIR1bits.TMR2IF) {
        // count only in mode 2
        if(current_mode == 2) {
            tick_count++;
            if(tick_count >= delay_target) {
                counter++;
                if(counter > 15){
                    counter = 0;
                }
                LATD = (counter << 4); // RD4-RD7
                tick_count = 0;
            }
        }
        PIR1bits.TMR2IF = 0;
    }
}