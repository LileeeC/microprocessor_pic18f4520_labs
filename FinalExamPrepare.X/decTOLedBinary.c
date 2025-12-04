#include "setting.c"
#include <stdlib.h> // 為了使用 atoi (字串轉整數)
#include <string.h>

// 全域變數 (ISR 與 Main 共用)
volatile int input_ready = 0; // 旗標：0=沒事, 1=收到Enter
volatile char rx_buf[10];     // 暫存輸入的字串
volatile int rx_idx = 0;      // 目前存到第幾個字

void main(void) {
    // 1. 初始化
    UART_Initialize(); // 記得 Baud rate 設為 1200 或 9600
    LED_Initialize();  // 記得 TRISD = 0 (Output), LATD = 0
    
    // 開啟中斷
    RCONbits.IPEN = 1;
    INTCONbits.GIEH = 1;
    PIE1bits.RCIE = 1;
    IPR1bits.RCIP = 1;
    
    printf("\r\nPlease input number (0-15): ");
    
    int value = 0;

    while(1) {
        // 2. 檢查是否收到完整的輸入 (由 ISR 設定)
        if(input_ready) {
            input_ready = 0; // 清除旗標
            
            // 3. 字串轉整數
            value = atoi(rx_buf);
            
            // 4. 範圍檢查 (0 ~ 15)
            if(value >= 0 && value <= 15) {
                // 5. 顯示二進位
                // 15 (1111) -> RD3, RD2, RD1, RD0 全亮
                // 使用 & 0x0F 確保只影響低 4 位，不影響 RD4-RD7
                LATD = (LATD & 0xF0) | (value & 0x0F);
                
                printf("\r\nOK! Value: %d, Binary: ", value);
                // 印出二進位給使用者看 (Optional)
                putch((value & 8) ? '1' : '0');
                putch((value & 4) ? '1' : '0');
                putch((value & 2) ? '1' : '0');
                putch((value & 1) ? '1' : '0');
                printf("\r\n");
            } 
            else {
                printf("\r\nError: Range must be 0-15\r\n");
                LATD = 0; // 錯誤時全滅，或是閃爍警告
            }
            
            printf("Input number (0-15): ");
        }
    }
}

// --- UART 接收中斷 ---
void __interrupt(high_priority) Hi_ISR(void) {
    if(PIR1bits.RCIF) {
        // 處理 Overflow 錯誤 (必備，不然 UART 會卡死)
        if(RCSTAbits.OERR) {
            RCSTAbits.CREN = 0; Nop(); RCSTAbits.CREN = 1;
        }

        char c = RCREG;
        
        // 判斷是否為 Enter 鍵 (\r 或 \n)
        if(c == '\r' || c == '\n') {
            if(rx_idx > 0) { // 確保有輸入東西
                rx_buf[rx_idx] = '\0'; // 補上結尾符號，變成合法字串
                input_ready = 1;       // 通知 Main 處理
                rx_idx = 0;            // 重置索引
            }
        } 
        else {
            // 只接收數字 (0-9)
            if(c >= '0' && c <= '9') {
                if(rx_idx < 9) { // 防止 Buffer 爆掉
                    rx_buf[rx_idx++] = c;
                    putch(c); // 回顯 (讓使用者看到自己打的字)
                }
            }
        }
        PIR1bits.RCIF = 0;
    }
}