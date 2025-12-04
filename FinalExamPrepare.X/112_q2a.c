//Question 2a (Button + Motor)
//Mode 1: 馬達在 -45° 到 +45° 之間來回轉動。
//Mode 2: 馬達在 -90° 到 +90° 之間來回轉動。
//切換機制: 系統預設 Mode 1，按下按鈕 (RB0) 後切換模式。
//注意: 必須在轉動迴圈中不斷檢查按鈕，才能即時反應。

#include "setting_hardaware/setting.h"
#include <stdlib.h>
#include "stdio.h"
#include "string.h"

// *** 重要：馬達需要 16ms 週期，需使用 1MHz ***
#define _XTAL_FREQ 1000000 

// Servo 角度對應 (1MHz, Prescaler 16)
#define DEG_N90  31   // -90度
#define DEG_N45  62   // -45度
#define DEG_0    93   // 0度
#define DEG_P45  125  // +45度
#define DEG_P90  150  // +90度

void PWM_Initialize(void) {
    // Timer2 設定給 PWM 使用
    T2CONbits.TMR2ON = 1;
    T2CONbits.T2CKPS = 0b11; // Prescaler 16
    PR2 = 0xFF;              // Period 設為最大 (約 16ms)
    
    // CCP1 設為 PWM 模式
    CCP1CONbits.CCP1M = 0b1100;
    TRISCbits.TRISC2 = 0;    // RC2 Output
}

void set_servo_duty(int duty) {
    CCPR1L = duty >> 2;
    CCP1CONbits.DC1B = duty & 0x03;
}

void main(void) {
    // 初始化
    OSCCONbits.IRCF = 0b100; // 設為 1MHz
    ADC_Initialize();        // 把 AN0 轉數位，避免干擾
    TRISBbits.TRISB0 = 1;    // Button Input
    PWM_Initialize();        // 啟動 PWM
    
    int mode = 1;
    set_servo_duty(DEG_N45); // 初始位置

    while(1) {
        if (mode == 1) {
            // Mode 1: -45 -> +45 -> -45
            // 往上掃
            for(int i=DEG_N45; i<=DEG_P45; i++) {
                set_servo_duty(i);
                __delay_ms(15);
                // 檢查按鈕 (Polling)
                if(PORTBbits.RB0 == 0) {
                    __delay_ms(20);
                    if(PORTBbits.RB0 == 0) {
                        mode = 2; 
                        while(PORTBbits.RB0 == 0); // 等待放開
                        break; 
                    }
                }
            }
            if(mode == 2) continue; // 若切換了就跳過下面動作

            // 往下掃
            for(int i=DEG_P45; i>=DEG_N45; i--) {
                set_servo_duty(i);
                __delay_ms(15);
                if(PORTBbits.RB0 == 0) {
                    __delay_ms(20);
                    if(PORTBbits.RB0 == 0) {
                        mode = 2; 
                        while(PORTBbits.RB0 == 0);
                        break; 
                    }
                }
            }
        } 
        else {
            // Mode 2: -90 -> +90 -> -90
            // 往上掃
            for(int i=DEG_N90; i<=DEG_P90; i++) {
                set_servo_duty(i);
                __delay_ms(15);
                if(PORTBbits.RB0 == 0) {
                    __delay_ms(20);
                    if(PORTBbits.RB0 == 0) {
                        mode = 1; 
                        while(PORTBbits.RB0 == 0);
                        break; 
                    }
                }
            }
            if(mode == 1) continue;

            // 往下掃
            for(int i=DEG_P90; i>=DEG_N90; i--) {
                set_servo_duty(i);
                __delay_ms(15);
                if(PORTBbits.RB0 == 0) {
                    __delay_ms(20);
                    if(PORTBbits.RB0 == 0) {
                        mode = 1; 
                        while(PORTBbits.RB0 == 0);
                        break; 
                    }
                }
            }
        }
    }
}