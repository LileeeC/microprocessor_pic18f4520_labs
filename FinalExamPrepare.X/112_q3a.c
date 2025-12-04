//Question 3a (VR Control Motor 0°~90°)
//使用 VR (ADC 0~1023) 對應馬達角度 (0° ~ 90°)。
//Mapping (映射)：ADC = 0 -> 0°, ADC = 1023 -> 90°
//馬達參數 (基於先前設定 Fosc=1MHz, Prescaler=16)：0° (1.5ms) = Count 93, 90° (2.4ms) = Count 150

#include "setting_hardaware/setting.h"
#include <stdlib.h>
#include "stdio.h"
#include "string.h"

// 必須使用 1MHz 以符合 Servo PWM 週期 (約 16ms)
#define _XTAL_FREQ 1000000

// Servo 角度對應 (Count = Time_us / 16us)
// 0度 (約 1.5ms = 1500us) -> 93.75 -> 93
// 90度 (約 2.4ms = 2400us) -> 150
#define DEG_0   93
#define DEG_90  150

void PWM_Initialize(void) {
    T2CONbits.TMR2ON = 1;
    T2CONbits.T2CKPS = 0b11; // Prescaler 16
    PR2 = 0xFF;              // Period Max
    CCP1CONbits.CCP1M = 0b1100; // PWM mode
    TRISCbits.TRISC2 = 0;    // RC2 Output
}

void set_servo_duty(int duty) {
    CCPR1L = duty >> 2;
    CCP1CONbits.DC1B = duty & 0x03;
}

int ADC_Read(void) {
    ADCON0bits.GO = 1;
    while(ADCON0bits.GO);
    return (ADRESH << 8) + ADRESL;
}

void main(void) {
    // 1. 初始化
    OSCCONbits.IRCF = 0b100; // 1MHz
    ADC_Initialize();        // VR 輸入
    PWM_Initialize();        // 馬達輸出
    
    int vr_value = 0;
    int duty_cycle = DEG_0;

    while(1) {
        // 2. 讀取 VR (0 ~ 1023)
        vr_value = ADC_Read();

        // 3. 計算對應的 Duty Cycle (線性映射)
        // 公式：Output = Min + (Input * Range_Out / Range_In)
        // Range_Out = 150 - 93 = 57
        // Range_In  = 1023
        // 使用 long 強制轉型避免 overflow
        duty_cycle = DEG_0 + (int)((long)vr_value * (DEG_90 - DEG_0) / 1023);

        // 4. 輸出 PWM
        set_servo_duty(duty_cycle);

        // 5. 稍微延遲 (讓 AD 轉換跟馬達有時間反應)
        __delay_ms(10);
    }
}