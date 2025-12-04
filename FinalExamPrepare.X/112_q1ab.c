// Q1a & Q1b (VR + LED + UART)
// 讀 VR 切換三種閃爍頻率 (1s, 0.5s, 0.25s)
// 每次閃爍時，UART 回傳狀態與累計次數 State_X count = n
#include "my_setting.c"

// 補完 Putty 輸出用
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

void main(void) {
    // 1. 初始化
    UART_Initialize(); // 記得檢查 Baud rate 是否為 1200
    ADC_Initialize();
    LED_Initialize();
    
    int vr_value = 0;
    int state = 0;
    int delay_ms_time = 0;
    int flash_count = 0; // 累計次數
    
    printf("\r\nQuestion 1 Start\r\n");

    while(1) {
        // 讀取 VR (0-1023)
        vr_value = ADC_Read();
        
        // 判斷區間與狀態
        if(vr_value < 341) {
            state = 1;
            // 頻率 1Hz (1s) => 半週期 500ms
            delay_ms_time = 500;
        } else if (vr_value < 682) {
            state = 2;
            // 頻率 2Hz (0.5s) => 半週期 250ms
            delay_ms_time = 250;
        } else {
            state = 3;
            // 頻率 4Hz (0.25s) => 半週期 125ms
            delay_ms_time = 125;
        }
        
        // 累計次數
        flash_count++;
        
        // Q1b: 輸出格式
        printf("State_%d count = %d\r\n", state, flash_count);
        
        // LED 亮
        LATDbits.LATD0 = 1; // 假設 LED 接在 RD0
        for(int i=0; i<delay_ms_time; i++) __delay_ms(1);
        
        // LED 滅
        LATDbits.LATD0 = 0;
        for(int i=0; i<delay_ms_time; i++) __delay_ms(1);
    }
}