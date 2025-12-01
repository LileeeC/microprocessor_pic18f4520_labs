#ifndef LIGHT_CONTROL_H
#define LIGHT_CONTROL_H

#include <xc.h>
#include <stdint.h>

// 定義亮度等級 (0~5)
#define BRIGHTNESS_OFF 0
#define BRIGHTNESS_MAX 5

// 定義模式
typedef enum {
    MODE_STATIC,    // 恆亮模式 (閱讀)
    MODE_BREATHING, // 呼吸燈模式 (氣氛)
    MODE_OFF        // 關閉
} LightMode_t;

// --- 函式宣告 ---
void PWM_Init(void);                      // 初始化 PWM
void Light_SetMode(LightMode_t new_mode); // 設定模式 (A 呼叫這支)
void Light_SetLevel(uint8_t level);       // 設定亮度等級 (0-5)
void Light_Update(void);                  // 放在 main 的 while(1) 裡執行特效

#endif