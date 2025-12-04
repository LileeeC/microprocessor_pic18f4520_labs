//Question 1a (LED State Machine)
//按鈕切換三種狀態，控制 LED 流水燈的速度 (0.25s, 0.5s, 1s)。
//流水燈：三顆 LED 輪流亮 (順序參考圖示，通常是左到右或右到左)。

#include "setting.c"

void main(void) {
    // 初始化
    Button_Initialize(); // RB0 Input
    LED_Initialize();    // RD0-RD3 Output
    
    // 變數
    int state = 1;       // 初始 State 1
    int delay_base = 250;// State 1: 0.25s = 250ms
    
    // LED 跑馬燈變數 (使用 0, 1, 2 代表哪顆亮)
    int led_index = 0; 

    while(1) {
        // ------------------------------------
        // 1. 執行 LED 動作
        // ------------------------------------
        LATD = 0; // 先全滅
        
        // 依照 index 點亮 LED (假設接在 RD0, RD1, RD2)
        if(led_index == 0) LATDbits.LATD0 = 1;
        else if(led_index == 1) LATDbits.LATD1 = 1;
        else if(led_index == 2) LATDbits.LATD2 = 1;
        
        // 更新 index (0->1->2->0...)
        led_index++;
        if(led_index > 2) led_index = 0;

        // ------------------------------------
        // 2. 延遲與按鈕偵測 (Polling)
        // ------------------------------------
        // 將長時間 delay 拆成多次短 delay，以便隨時偵測按鈕
        for(int i=0; i < delay_base; i++) {
            __delay_ms(1); 
            
            // 檢查按鈕
            if(PORTBbits.RB0 == 0) {
                __delay_ms(20); // Debounce
                if(PORTBbits.RB0 == 0) {
                    // 切換狀態
                    state++;
                    if(state > 3) state = 1;
                    
                    // 更新時間參數
                    if(state == 1) delay_base = 250;      // 0.25s
                    else if(state == 2) delay_base = 500; // 0.5s
                    else if(state == 3) delay_base = 1000;// 1s
                    
                    while(PORTBbits.RB0 == 0); // 等待放開
                }
            }
        }
    }
}