//Question 2a (Motor Modes)
//Mode 1：+90° <-> -90° (來回掃描)。
//Mode 2：+90° $\leftrightarrow$ 0° (來回掃描)
//切換：按鈕切換模式，馬達需立即反應。

#include "setting.c"

// 角度對應 (Fosc=1MHz, Prescaler=16)
#define DEG_N90 31   // -90度
#define DEG_0   93   // 0度
#define DEG_P90 150  // +90度

void set_servo(int duty) {
    CCPR1L = duty >> 2;
    CCP1CONbits.DC1B = duty & 0x03;
}

void main(void) {
    Button_Initialize();
    PWM_Initialize();
    
    int mode = 1;
    
    while(1) {
        // ------------------------
        // Mode 1: +90 <-> -90
        // ------------------------
        if(mode == 1) {
            // 往 -90 轉
            for(int i=DEG_P90; i>=DEG_N90; i--) {
                set_servo(i);
                __delay_ms(10);
                // 檢查按鈕
                if(PORTBbits.RB0 == 0) {
                    __delay_ms(20);
                    if(PORTBbits.RB0==0) { mode=2; while(PORTBbits.RB0==0); break; }
                }
            }
            if(mode==2) continue; // 若切換則跳出
            
            // 往 +90 轉
            for(int i=DEG_N90; i<=DEG_P90; i++) {
                set_servo(i);
                __delay_ms(10);
                if(PORTBbits.RB0 == 0) {
                    __delay_ms(20);
                    if(PORTBbits.RB0==0) { mode=2; while(PORTBbits.RB0==0); break; }
                }
            }
        }
        // ------------------------
        // Mode 2: +90 <-> 0
        // ------------------------
        else {
            // 往 0 轉
            for(int i=DEG_P90; i>=DEG_0; i--) {
                set_servo(i);
                __delay_ms(10);
                if(PORTBbits.RB0 == 0) {
                    __delay_ms(20);
                    if(PORTBbits.RB0==0) { mode=1; while(PORTBbits.RB0==0); break; }
                }
            }
            if(mode==1) continue;
            
            // 往 +90 轉
            for(int i=DEG_0; i<=DEG_P90; i++) {
                set_servo(i);
                __delay_ms(10);
                if(PORTBbits.RB0 == 0) {
                    __delay_ms(20);
                    if(PORTBbits.RB0==0) { mode=1; while(PORTBbits.RB0==0); break; }
                }
            }
        }
    }
}