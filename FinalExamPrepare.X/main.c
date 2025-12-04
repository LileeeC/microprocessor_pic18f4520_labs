/#include<my_setting.c>
#include <xc.h>

// 實作 putch 供 printf 使用
void putch(char data) {
    while (!TXSTAbits.TRMT); // 等待 TX 緩衝區淨空
    TXREG = data;
}

// 實作 delay 函數 (以秒為單位)
int delay(double sec) {
    // 簡單實作，利用內建 __delay_ms
    // 因為 __delay_ms 參數有限制，拆成小迴圈
    int ms = (int)(sec * 1000);
    for(int i=0; i<ms; i++) {
        __delay_ms(1);
    }
    return 1;
}

// 讀取 ADC 數值 (0-1023)
int ADC_Read(void) {
    ADCON0bits.GO = 1; // 開始轉換
    while (ADCON0bits.GO); // 等待轉換完成
    return (ADRESH << 8) + ADRESL; // 回傳 10-bit 結果
}

// 設定 PWM Duty Cycle (給伺服馬達用)
// 針對 Servo: -90度 ~ 0.5ms, 0度 ~ 1.5ms, +90度 ~ 2.4ms
// 在 1MHz, Prescaler 16 下，Timer2 每一 tick = 16us
// 10-bit PWM 解析度下: 
// 0.5ms = 500us / 16us = 31 counts
// 1.5ms = 1500us / 16us = 93 counts
// 2.4ms = 2400us / 16us = 150 counts
void set_servo_duty(unsigned int duty_counts) {
    CCPR1L = duty_counts >> 2;   // 高 8 bits
    CCP1CONbits.DC1B = duty_counts & 0x03; // 低 2 bits
}

void main(void) {
    return;
}
