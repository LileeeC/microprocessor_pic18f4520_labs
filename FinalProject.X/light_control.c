#include "light_control.h"

// 內部變數
static LightMode_t current_mode = MODE_STATIC;
static uint8_t current_level = 0; // 0-5
static uint16_t pwm_duty = 0;     // 0-1023 (10-bit PWM)

// 呼吸燈專用變數
static int breath_dir = 1;        // 1: 變亮, -1: 變暗
static uint16_t breath_val = 0;
static uint8_t delay_counter = 0; // 用來減慢呼吸速度

void PWM_Init(void) {
    // 1. 設定 RC2 為輸出
    TRISCbits.TRISC2 = 0; 
    
    // 2. 設定 PWM 週期 (配合 20MHz 晶振)
    // Timer2 Prescaler = 16, PR2 = 255 -> Freq 約 1.2kHz
    PR2 = 255; 
    
    // 3. 設定 CCP1 模組為 PWM 模式
    CCP1CONbits.CCP1M = 0b1100; 
    
    // 4. 設定 Timer2 Prescaler = 16 並開啟
    T2CONbits.T2CKPS = 0b10; // Prescaler 16
    T2CONbits.TMR2ON = 1;    // 啟動 Timer2
}

// 設定最底層的 Duty Cycle (0 ~ 1023)
void PWM_Set_Duty(uint16_t duty) {
    if (duty > 1023) duty = 1023;
    
    // 10-bit PWM: 高 8 bit 放 CCPR1L，低 2 bit 放 CCP1CON<5:4>
    CCPR1L = duty >> 2;
    CCP1CONbits.DC1B = duty & 0x03;
}

void Light_SetLevel(uint8_t level) {
    if (level > BRIGHTNESS_MAX) level = BRIGHTNESS_MAX;
    current_level = level;
    
    // 將 0-5 轉換成 0-1023
    // 0->0, 1->200, 2->400... 5->1000
    uint16_t target_duty = level * 200; 
    PWM_Set_Duty(target_duty);
}

void Light_SetMode(LightMode_t new_mode) {
    current_mode = new_mode;
    if (new_mode == MODE_STATIC) {
        Light_SetLevel(current_level); // 恢復成原本亮度
    } else if (new_mode == MODE_OFF) {
        PWM_Set_Duty(0);
    }
}

// ★ 這個函式很重要，A 必須在他的 while(1) 裡一直呼叫它
// 這樣你的呼吸燈才會動，而且不會卡住 B 的感測器
void Light_Update(void) {
    if (current_mode == MODE_BREATHING) {
        delay_counter++;
        if (delay_counter > 10) { // 每呼叫 10 次才變一次，調整呼吸速度
            delay_counter = 0;
            
            breath_val += (breath_dir * 10); // 每次加減 10
            
            if (breath_val >= 1000) {       // 到頂變暗
                breath_val = 1000;
                breath_dir = -1;
            } else if (breath_val <= 10) {  // 到底變亮
                breath_val = 10;
                breath_dir = 1;
            }
            PWM_Set_Duty(breath_val);
        }
    }
}