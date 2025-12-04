//Question 2b (UART + Motor)
//UART 控制: 使用者輸入字串 "mode1" 或 "mode2"。
//Mode 1: -90° 轉到 +90°，然後 Reset (快速/直接) 回到 -90°。
//Mode 2: -90° 轉到 +90°，然後 Smoothly decrease (慢速) 回到 -90°。
//Note: Mode 2 的回程 (90->-90) 必須比 Mode 1 明顯更慢 (Add delay)。

#include "setting_hardaware/setting.h"
#include <stdlib.h>
#include "stdio.h"
#include "string.h"

#define _XTAL_FREQ 1000000 

// 引用外部 uart.c 的變數 (假設你有定義)
extern void MyusartRead();
extern char *GetString();
extern void ClearBuffer();

// 全域變數
volatile int current_mode = 1; // 1 or 2
char rx_buffer[20];
int rx_index = 0;

// Servo 定義
#define DEG_N90  31
#define DEG_P90  150

void PWM_Initialize(void) {
    T2CONbits.TMR2ON = 1;
    T2CONbits.T2CKPS = 0b11; // Prescaler 16
    PR2 = 0xFF;              // Period Max
    CCP1CONbits.CCP1M = 0b1100;
    TRISCbits.TRISC2 = 0;    
}

void set_servo_duty(int duty) {
    CCPR1L = duty >> 2;
    CCP1CONbits.DC1B = duty & 0x03;
}

void main(void) {
    OSCCONbits.IRCF = 0b100; // 1MHz
    UART_Initialize();       // 設定 UART, 9600 or 1200
    PWM_Initialize();        // 設定 PWM
    
    // 開啟中斷 (只開 UART RX)
    RCONbits.IPEN = 1;
    INTCONbits.GIEH = 1;
    PIE1bits.RCIE = 1;
    IPR1bits.RCIP = 1;       // High Priority
    
    UART_Write_Text("\r\nQ2b Start. Type mode1 or mode2\r\n");

    while(1) {
        // 不管 Mode 1 或 2，前半段都是 -90 掃到 +90
        for(int i=DEG_N90; i<=DEG_P90; i++) {
            set_servo_duty(i);
            __delay_ms(15);
        }
        
        // 根據 ISR 改動的 current_mode 決定回程方式
        if(current_mode == 1) {
            // Mode 1: Reset to -90 (Fast)
            // 題目: "resets to -90" -> 快速歸零
            set_servo_duty(DEG_N90);
            __delay_ms(200); // 給一點時間讓馬達轉回去
        } 
        else {
            // Mode 2: Smoothly decrease (Slow)
            // 題目: "visibly slower than Mode 1"
            for(int i=DEG_P90; i>=DEG_N90; i--) {
                set_servo_duty(i);
                __delay_ms(40); // 故意設慢一點 (40ms)
            }
        }
        
        __delay_ms(100); // 稍微停頓
    }
}

// High Priority Interrupt for UART
void __interrupt(high_priority) Hi_ISR(void) {
    if(PIR1bits.RCIF) {
        // 處理 OERR (Buffer Overflow)
        if(RCSTAbits.OERR) {
            RCSTAbits.CREN = 0; 
            Nop(); 
            RCSTAbits.CREN = 1;
        }

        char c = RCREG;
        
        // 簡單的字串接收與判斷 (取代複雜的 GetString)
        if(c == '\r' || c == '\n') {
            rx_buffer[rx_index] = '\0'; // 結尾
            
            // 判斷指令
            if(strcmp(rx_buffer, "mode1") == 0) {
                current_mode = 1;
                UART_Write_Text("\r\nMode 1 Set\r\n");
            } 
            else if(strcmp(rx_buffer, "mode2") == 0) {
                current_mode = 2;
                UART_Write_Text("\r\nMode 2 Set\r\n");
            }
            
            rx_index = 0; // 重置 buffer
        } 
        else {
            // 存入 buffer 並 echo
            if(rx_index < 15) {
                rx_buffer[rx_index++] = c;
                UART_Write(c); // Echo
            }
        }
        
        PIR1bits.RCIF = 0;
    }
}

// Low Priority ISR 必須移除或留空
// 因為 TMR2 給 PWM 用了，不能開中斷
void __interrupt(low_priority) Lo_ISR(void) {
    // Empty
}