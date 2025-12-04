//Question 2b (Complex Motor Logic)
//初始位置：-90° (依題目圖示範例)。
//按鈕觸發序列：轉 45° -> 轉 90° -> 轉 180° -> 重複。
//反轉機制：若碰到 +90° 或 -90° 邊界，則反轉方向並走完剩餘角度 (Bounce/Rebound 邏輯)。

#include "setting.c"
#include <stdlib.h> // for abs()

// 角度與 PWM Count 轉換係數 (150-31 = 119 counts for 180 degrees)
// 1 degree approx = 0.66 counts. 為了方便計算，我們用虛擬角度處理
// 虛擬角度: -90 ~ +90. 
// Mapping: PWM = 31 + (Angle + 90) * (119/180)

// 直接定義角度邊界
#define ANG_MIN -90
#define ANG_MAX 90

int angle_to_pwm(int ang) {
    // 線性映射: -90->31, +90->150，31和150根據馬達實際測試角度調整
    // formula: 31 + (ang + 90) * 119 / 180
    return 31 + (int)((long)(ang + 90) * 119 / 180);
}

void set_servo_angle(int ang) {
    int pwm = angle_to_pwm(ang);
    CCPR1L = pwm >> 2;
    CCP1CONbits.DC1B = pwm & 0x03;
}

void main(void) {
    Button_Initialize();
    PWM_Initialize();
    
    // 狀態變數
    int current_angle = -90; // 初始 -90
    int direction = 1;       // 1: CounterClockwise(+), -1: Clockwise(-)
    int step_index = 0;      // 0:45deg, 1:90deg, 2:180deg
    int steps[] = {45, 90, 180}; // 題目要求的步進序列
    
    set_servo_angle(current_angle);
    
    while(1) {
        if(PORTBbits.RB0 == 0) {
            __delay_ms(20);
            if(PORTBbits.RB0 == 0) {
                // 1. 取得這次要轉的角度量
                int move_amount = steps[step_index];
                
                // 2. 更新序列索引 (0->1->2->0...)
                step_index++;
                if(step_index > 2) step_index = 0;
                
                // 3. 計算目標與反彈邏輯
                // 模擬移動：目標 = 目前 + 方向 * 量
                int target = current_angle + (direction * move_amount);
                
                // 檢查是否超過邊界 (+90)
                if(target > ANG_MAX) {
                    int overshoot = target - ANG_MAX;
                    current_angle = ANG_MAX - overshoot; // 反彈
                    direction = -1; // 方向反轉
                }
                // 檢查是否超過邊界 (-90)
                else if(target < ANG_MIN) {
                    int overshoot = ANG_MIN - target;
                    current_angle = ANG_MIN + overshoot; // 反彈
                    direction = 1; // 方向反轉
                }
                else {
                    // 沒撞牆，直接移動
                    current_angle = target;
                }
                
                // 4. 執行轉動
                set_servo_angle(current_angle);
                
                // 等待放開
                while(PORTBbits.RB0 == 0); 
                __delay_ms(200); // 稍微延遲避免連點
            }
        }
    }
}