#include "setting.c"
#include <stdlib.h>
#include <stdio.h>

// 定義 7 種 LED 狀態 (0:D, 1:L)
// 為了符合範例的 LLL, DLL... 我們建立 Pattern 表
// 假設 State 0-6 對應二進位 1-7
// State 0: 001 (DDL)
// State 1: 010 (DLD)
// State 2: 011 (DLL) ... etc
const int led_patterns[] = {1, 2, 4, 3, 5, 6, 7}; 
// 或者更簡單：Running Light 通常是 1, 2, 4 循環
// 但題目 Q3b 有 "LLL", "DDL" 這種組合，且公式 %7，暗示有 7 種 pattern。
// 最合理的假設是 binary 1 ~ 7。
// State counter 0~6 -> Value 1~7.

// 輸出單字元
void putch(char data) {
    while (!TXSTAbits.TRMT);
    TXREG = data;
}

// 讀取 ADC
int ADC_Read(void) {
    ADCON0bits.GO = 1;
    while(ADCON0bits.GO);
    return (ADRESH << 8) + ADRESL;
}

// 顯示 LED 狀態字串 (如 "LDD")
// val: 3-bit integer (e.g. 5 = 101 = LDL)
void PrintState(int val) {
    // Bit 2
    putch((val & 4) ? 'L' : 'D');
    // Bit 1
    putch((val & 2) ? 'L' : 'D');
    // Bit 0
    putch((val & 1) ? 'L' : 'D');
    putch('\r'); putch('\n');
}

// 全域變數供 ISR 使用
volatile long x_input = 0;
volatile char rx_buf[15];
volatile int rx_idx = 0;
volatile int input_ready = 0;

void __interrupt(high_priority) Hi_ISR(void) {
    if(PIR1bits.RCIF) {
        char c = RCREG;
        if(c == '\r' || c == '\n') {
            rx_buf[rx_idx] = '\0';
            x_input = atol(rx_buf); // 轉長整數
            input_ready = 1;        // 標記輸入完成
            rx_idx = 0;
        } else {
            if(rx_idx < 14) rx_buf[rx_idx++] = c;
        }
        PIR1bits.RCIF = 0;
    }
}

void main(void) {
    UART_Initialize();
    ADC_Initialize();
    LED_Initialize();
    
    // 開啟中斷
    RCONbits.IPEN = 1; 
    INTCONbits.GIEH = 1; 
    PIE1bits.RCIE = 1; 
    IPR1bits.RCIP = 1;
    
    int current_state_idx = 0; // 0 ~ 6
    int vr_val = 0;
    int direction = 1; // 1: Right, -1: Left
    
    while(1) {
        // -----------------------
        // Q3b Function 2: Check UART Input
        // -----------------------
        if(input_ready) {
            input_ready = 0; // Clear flag
            
            // update state: (state + X) % 7
            // 注意 state 是 index (0-6)
            // 公式: new = (old + x) % 7
            current_state_idx = (current_state_idx + x_input) % 7;
            
            // 顯示更新後的狀態
            int pattern = current_state_idx + 1; // 轉成 1~7 數值
            PrintState(pattern);
        }
        
        // -----------------------
        // Q3a: VR Control Direction
        // -----------------------
        vr_val = ADC_Read();
        if(vr_val > 512) direction = 1;  // 向右
        else direction = -1;             // 向左 (或依照題目 Left to Right 定義)
        
        // 只有在沒有輸入時才跑馬燈? 
        // 題目 Q3b-2: "VR should remain stationary while inputting".
        // 這暗示我們不需要在輸入時暫停程式，而是輸入完成後才跳變。
        // 平常則依據 VR 跑動。
        
        // 正常跑馬燈更新
        current_state_idx += direction;
        
        // 邊界處理 (0 ~ 6 循環)
        if(current_state_idx > 6) current_state_idx = 0;
        if(current_state_idx < 0) current_state_idx = 6;
        
        // 點亮 LED
        // 假設狀態對應: State 0 -> Binary 1 (001), State 6 -> Binary 7 (111)
        int pattern = current_state_idx + 1; 
        LATD = pattern; 
        
        // Q3b Function 1: Display 'L'/'D' result
        // 為了避免 UART 刷太快，建議只有狀態改變或每隔一段時間印一次
        // 這裡每步都印 (Putty 會跑很快)
        PrintState(pattern); 
        
        __delay_ms(500); // 跑馬燈速度
    }
}