//Question 1b (UART Countdown)
//UART 輸入：兩個數字，用空白隔開 (例如 "2 7")。
//第一個數字：State (1~3)，決定倒數速度。
//第二個數字：倒數起始值 (n)。
//倒數顯示：用 3 顆 LED 顯示二進位數值，從 n 數到 0。

#include "setting.c"
#include <stdlib.h>
#include <string.h>

// 輸出字元
void putch(char data) {
    while (!TXSTAbits.TRMT);
    TXREG = data;
}

// 接收字串 (直到換行)
void GetString(char *buf) {
    int i = 0;
    char c;
    while(1) {
        if(PIR1bits.RCIF) {
            c = RCREG;
            putch(c); // Echo
            if(c == '\r' || c == '\n') {
                buf[i] = '\0';
                break;
            }
            buf[i++] = c;
        }
    }
}

void main(void) {
    // 初始化
    UART_Initialize();
    LED_Initialize();
    
    char buffer[20];
    int state_input = 0;
    int start_n = 0;
    int delay_time = 500;
    
    printf("\r\nInput format: 'State N' (e.g., 2 7)\r\n");

    while(1) {
        // 1. 讀取輸入
        GetString(buffer);
        printf("\r\n");
        
        // 2. 解析字串 (利用 strtok 或手動解析)
        // 簡單做法：找到空格位置
        char *p1 = strtok(buffer, " ");
        char *p2 = strtok(NULL, " ");
        
        if(p1 != NULL && p2 != NULL) {
            state_input = atoi(p1);
            start_n = atoi(p2);
            
            // 設定速度
            if(state_input == 1) delay_time = 250;
            else if(state_input == 2) delay_time = 500;
            else if(state_input == 3) delay_time = 1000;
            
            // 3. 執行倒數
            for(int i = start_n; i >= 0; i--) {
                // 顯示二進位到 LED (RD0-RD2)
                LATD = (i & 0x07); // 取最低3位
                
                // 延遲
                for(int k=0; k<delay_time; k++) __delay_ms(1);
            }
            
            // 倒數結束，全滅或維持? 題目寫 -> END
            LATD = 0;
        }
    }
}