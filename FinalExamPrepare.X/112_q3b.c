//Question 3b (UART Input & Calculation)
//不需要控制馬達 (雖然接腳可能還在，但題目重點是 計算)。
//初始整數 x = 100。
//透過 UART 輸入新的數字來更新 x。
//在 Putty 顯示 VR數值 / x 的結果。
//使用 High Priority ISR 來處理 UART 輸入，避免卡住主迴圈。

#include "setting_hardaware/setting.h"
#include <stdlib.h>
#include "stdio.h"
#include "string.h"

#define _XTAL_FREQ 1000000

// 全域變數
volatile int x_divisor = 100; // 題目預設 100
char rx_buffer[10];
int rx_index = 0;

int ADC_Read(void) {
    ADCON0bits.GO = 1;
    while(ADCON0bits.GO);
    return (ADRESH << 8) + ADRESL;
}

void main(void) {
    // 1. 初始化
    OSCCONbits.IRCF = 0b100; // 1MHz
    ADC_Initialize();        // VR 輸入
    UART_Initialize();       // 傳輸與接收
    
    // 2. 開啟 UART 接收中斷
    RCONbits.IPEN = 1;       // Enable Priorities
    INTCONbits.GIEH = 1;     // Global Interrupt High
    PIE1bits.RCIE = 1;       // UART RX Interrupt Enable
    IPR1bits.RCIP = 1;       // High Priority
    
    UART_Write_Text("\r\nQ3b Start. Initial x = 100\r\n");

    int vr_value = 0;
    int result = 0;
    char str_buf[30];

    while(1) {
        // 3. 讀取 VR
        vr_value = ADC_Read();
        
        // 4. 執行計算 (避免除以 0)
        if (x_divisor != 0) {
            result = vr_value / x_divisor;
        } else {
            result = 0; // Error handling
        }

        // 5. 顯示結果 (格式依題目要求，這裡範例顯示 VR, x, Result)
        // 使用 sprintf 組合字串比較保險
        sprintf(str_buf, "VR:%4d / x:%3d = %3d\r", vr_value, x_divisor, result);
        UART_Write_Text(str_buf);
        
        // 6. 延遲 (避免畫面刷太快看不到數值變化)
        __delay_ms(200);
    }
}

// ------------------------------------------------------------------
// High Priority ISR: 處理使用者輸入數字
// ------------------------------------------------------------------
void __interrupt(high_priority) Hi_ISR(void) {
    if(PIR1bits.RCIF) {
        // 錯誤處理 (Overrun Error)
        if(RCSTAbits.OERR) {
            RCSTAbits.CREN = 0; Nop(); RCSTAbits.CREN = 1;
        }

        char c = RCREG;

        // 判斷 Enter 鍵 (\r 或 \n) 表示輸入完成
        if(c == '\r' || c == '\n') {
            rx_buffer[rx_index] = '\0'; // 字串結尾
            
            // 將字串轉為整數 (atoi)
            if (rx_index > 0) {
                int new_val = atoi(rx_buffer);
                
                // 題目範圍限制 (假設 10 ~ 600)
                if(new_val >= 10 && new_val <= 600) {
                    x_divisor = new_val;
                    UART_Write_Text("\r\nUpdated!\r\n");
                } else {
                    UART_Write_Text("\r\nRange Error (10-600)\r\n");
                }
            }
            rx_index = 0; // 重置 Buffer
        }
        else {
            // 只接收數字 (0-9)
            if(c >= '0' && c <= '9') {
                if(rx_index < 9) {
                    rx_buffer[rx_index++] = c;
                    UART_Write(c); // Echo 回顯給使用者看
                }
            }
        }
        PIR1bits.RCIF = 0;
    }
}