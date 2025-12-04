// CONFIG1H
#pragma config OSC = INTIO67  // Oscillator Selection bits (HS oscillator)
#pragma config FCMEN = OFF    // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = ON      // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 3         // Brown Out Reset Voltage bits (Minimum setting)

// CONFIG2H
#pragma config WDT = OFF  // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 1  // Watchdog Timer Postscale Select bits (1:1)

// CONFIG3H
#pragma config CCP2MX = PORTC  // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = ON     // PORTB A/D Enable bit (PORTB<4:0> pins are configured as analog input channels on Reset)
#pragma config LPT1OSC = OFF   // Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON      // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON  // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF    // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled)
#pragma config XINST = OFF  // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF  // Code Protection bit (Block 0 (000800-001FFFh) not code-protected)
#pragma config CP1 = OFF  // Code Protection bit (Block 1 (002000-003FFFh) not code-protected)
#pragma config CP2 = OFF  // Code Protection bit (Block 2 (004000-005FFFh) not code-protected)
#pragma config CP3 = OFF  // Code Protection bit (Block 3 (006000-007FFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF  // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF  // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF  // Write Protection bit (Block 0 (000800-001FFFh) not write-protected)
#pragma config WRT1 = OFF  // Write Protection bit (Block 1 (002000-003FFFh) not write-protected)
#pragma config WRT2 = OFF  // Write Protection bit (Block 2 (004000-005FFFh) not write-protected)
#pragma config WRT3 = OFF  // Write Protection bit (Block 3 (006000-007FFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF  // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF  // Boot Block Write Protection bit (Boot block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF  // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF  // Table Read Protection bit (Block 0 (000800-001FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF  // Table Read Protection bit (Block 1 (002000-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF  // Table Read Protection bit (Block 2 (004000-005FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF  // Table Read Protection bit (Block 3 (006000-007FFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF  // Boot Block Table Read Protection bit (Boot block (000000-0007FFh) not protected from table reads executed in other blocks)

#include <ctype.h>
#include <pic18f4520.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xc.h>
#define _XTAL_FREQ 1000000
#define STR_MAX 100
#define VR_MAX ((1 << 10) - 1)
// #define delay(t) __delay_ms(t * 1000);

//---------------- Global Variebles --------------------

char buffer[STR_MAX];
int buffer_size = 0;
bool btn_interr = false;

//---------------- Functions --------------------

// ===== UART functions =====
void putch(char data);
void ClearBuffer();
void MyusartRead();
int GetString(char *str);
void clear_terminal();

// ===== LED functions =====
int get_LED();
void set_LED(int value);
void set_LED_separately(int a, int b, int c, int d);
void set_LED_analog(int value);

// ===== Servo functions =====
int get_servo_angle();
int set_servo_angle(int angle);
int VR_value_to_servo_angle(int value);
int VR_value_to_LED_analog(int value);

// ===== Callback functions =====
void variable_register_changed(int value);
void button_pressed();
void keyboard_input(char *str);

// ===== Timer Callback functions =====
void timer0_interrupted(void);
void timer1_interrupted(void);
void timer2_interrupted(void); 
void timer3_interrupted(void);

// ===== Utility =====
int delay(double sec);

// ---------------- Settings --------------------

void TMR0_Initialize(void){
    // Interrupt Every 0.1 sec (When Fosc = 1M)
    INTCONbits.TMR0IF = 0;      // Clear the Timer0 interrupt flag
    INTCON2bits.TMR0IP = 0;     // Set Timer0 interrupt priority to low (1 = high, 0 = low)
    INTCONbits.TMR0IE = 1;      // Enable Timer0 interrupt

    T0CONbits.T08BIT = 0;       // 0 = 16-bit, 1 = 8-bit timer, 8 bit only uses TMR0L
    T0CONbits.T0PS = 0b000;     // Timer0 prescaler = 1:2 
    T0CONbits.T0CS = 0b0;       // Timer0 clock source = internal clock (Fosc/4)
    T0CONbits.T0SE = 0b0;       // Increase on rising edge
    T0CONbits.PSA = 0b0;        // Prescaler Enabled
    
    TMR0 = 53035;               //update in interrupt vector as well(Start here to 65535(16bits))
    T0CONbits.TMR0ON = 1;       //open timer0
}

void TMR1_Initialize(void){
    // Interrupt Every 0.25 sec (When Fosc = 1M)
    PIR1bits.TMR1IF = 0;   // Clear the Timer1 interrupt flag
    IPR1bits.TMR1IP = 0;   // Set Timer1 interrupt priority to low (1 = high, 0 = low)
    PIE1bits.TMR1IE = 1;   // Enable Timer1 interrupt 

    T1CONbits.T1CKPS = 0b01;  // Timer1 prescaler = 1:2 
    T1CONbits.TMR1CS = 0b0;   // Timer1 clock source = internal clock (Fosc/4)
    
    TMR1 = 34285; //update in interrupt vector as well(Start here to 65535(16bits))
    T1CONbits.TMR1ON = 1; //open timer1
}

void TMR2_Initialize(void){
    // Interrupt Every 0.5 sec (When Fosc = 250K)
    PIR1bits.TMR2IF = 0;   // Clear the Timer2 interrupt flag
    IPR1bits.TMR2IP = 0;   // Set Timer2 interrupt priority to low (1 = high, 0 = low)
    PIE1bits.TMR2IE = 1;   // Enable Timer2 interrupt 
    
    T2CONbits.T2CKPS = 0b11;   // Timer2 prescaler = 1:16 
    T2CONbits.T2OUTPS = 0b1111;  // Timer2 postscaler = 1:16 
    
    PR2 = 122; //start from 0 to here (MAX 255(8bits))
    T2CONbits.TMR2ON = 1; //open timer2
    
}

void TMR3_Initialize(void){
    // Interrupt Every 0.05 sec (When Fosc = 1M)
    PIR2bits.TMR3IF = 0;   // Clear the Timer3 interrupt flag
    IPR2bits.TMR3IP = 0;   // Set Timer3 interrupt priority to low (1 = high, 0 = low)
    PIE2bits.TMR3IE = 1;   // Enable Timer3 interrupt

    T3CONbits.T3CKPS = 0b01;  // Timer3 prescaler = 1:2 
    T3CONbits.TMR3CS = 0b0;   // Timer3 clock source = internal clock (Fosc/4)
    
    TMR3 = 59285; //update in interrupt vector as well(Start here to 65535(16bits))
    T3CONbits.TMR3ON = 1; //open timer3
}

void ADC_Initialize(void){
    // Configure ADC
    TRISAbits.RA0 = 1;         // Set RA0 as input port
    ADCON0bits.CHS = 0b0000;   // Select AN0 channel
    ADCON1bits.VCFG0 = 0;      // Vref+ = Vdd
    ADCON1bits.VCFG1 = 0;      // Vref- = Vss
    ADCON2bits.ADCS = 0b000;   // ADC clock Fosc/2 Tad = 2us
    ADCON2bits.ACQT = 0b001;   // 2Tad acquisition time 4us
    ADCON0bits.ADON = 1;       // Enable ADC
    ADCON2bits.ADFM = 1;       // Right justified
    PIR1bits.ADIF = 0;         // Clear ADC flag
    IPR1bits.ADIP = 0;         // Set ADC interrupt priority to low
    PIE1bits.ADIE = 1;         // Enable ADC interrupt
}

void PWM_Initialize(void){
    // Configure servo (PWM) 
    /*
     * PWM period
     * = (PR2 + 1) * 4 * Tosc * (TMR2 prescaler)
     * = (0xFF + 1) * 4 * 1µs * 16
     * = 16.32ms
     */
    T2CONbits.TMR2ON = 0b1;      // Timer2 on
    T2CONbits.T2CKPS = 0b11;     // Prescaler 16, no need to set postscaler

    PIE1bits.TMR2IE = 0;   // disable Timer2 interrupt

    CCP1CONbits.CCP1M = 0b1100;  // PWM mode
    PR2 = 0xFF;                  // Set PWM period

    TRISCbits.TRISC2 = 0;        // CCP1 as output for servo
    LATCbits.LATC2 = 0;          // Clear CCP1
}

void LED_Initialize(void){
    // Configure I/O ports
    TRISD &= 0x0F;  // Set RD4-RD7 as outputs for LED
    LATD &= 0x0F;   // Clear RD4-RD7
}

void Button_Initialize(void){
    INTCONbits.INT0IF = 0;  // Clear INT0 flag
    INTCONbits.INT0IE = 1;  // Enable INT0 interrupt
    //INTCON2bits.INTEDG0 = 1; // rising edge triggered (pressed)
    TRISB |= 0x01;      // RB0 as input for button
}

void UART_Initialize(void){
    // Configure UART
    /*
           TODObasic
           Serial Setting
        1.   Setting Baud rate
        2.   choose sync/async mode
        3.   enable Serial port (configures RX/DT and TX/CK pins as serial port pins)
        3.5  enable Tx, Rx Interrupt(optional)
        4.   Enable Tx & RX
    */
    TRISCbits.TRISC6 = 1;  // RC6(TX) : Transmiter set 1 (output)
    TRISCbits.TRISC7 = 1;  // RC7(RX) : Receiver set 1   (input)

    // Setting Baud rate (when fosc = 1M)
    // Baud rate = 1200 (Look up table)
    TXSTAbits.SYNC = 0;     // Synchronus or Asynchronus
    BAUDCONbits.BRG16 = 0;  // 16 bits or 8 bits
    TXSTAbits.BRGH = 0;     // High Baud Rate Select bit
    SPBRG = 12;             // Control the period
    //SPBRGH = ;            // Used when BRG16 = 1 

    // Serial enable
    RCSTAbits.SPEN = 1;  // Enable asynchronus serial port (must be set to 1)
    
    TXSTAbits.TXEN = 1;    // 啟動發送
    RCSTAbits.CREN = 1;    // 啟動接收
    
    PIR1bits.TXIF = 0;   // Set
}


// 清除 Putty 終端機畫面 (ANSI Escape Code)
//void clear_terminal() {
//    putch(27); // ESC
//    printf("[2J"); // Clear Screen
//    putch(27); // ESC
//    printf("[H");  // Cursor Home
//}

// 讓 printf 可以運作
//void putch(char data) {
//    while (!TXSTAbits.TRMT); // 等待發送緩衝區淨空
//    TXREG = data;
//}

// 接收字串 (Q1b, Q2b, Q3b 會用到)
// 讀取直到按下 Enter
//void GetString(char *buf) {
//    int i = 0;
//    char c;
//    while(1) {
//        if(PIR1bits.RCIF) {
//            c = RCREG;
//            putch(c); // Echo 回顯 (讓使用者看到自己打什麼)
//            
//            // 判斷 Enter (Windows 是 \r\n, Unix 是 \n, Putty 有時只送 \r)
//            if(c == '\r' || c == '\n') {
//                buf[i] = '\0'; // 補上結尾符號
//                break;         // 跳出
//            }
//            
//            // 防止緩衝區溢位 (假設 buffer 大小是 20)
//            if(i < 19) {
//                buf[i++] = c;
//            }
//        }
//    }
//}

// 設定 PWM Duty Cycle (直接控制暫存器)
// lab8 的 void set_motor_angle(unsigned int ten_bit)
//void set_servo_duty(int duty) {
//    CCPR1L = duty >> 2;             // 高 8 bits
//    CCP1CONbits.DC1B = duty & 0x03; // 低 2 bits
//}

// 如果你想直接輸入角度 (-90 ~ 90)，可以用這個轉換函數
// 假設頻率 1MHz, Prescaler 16
//void set_servo_angle(int angle) {
//    // 簡單線性映射: -90度(Count 31) ~ +90度(Count 150)
//    // 公式: Duty = 31 + (angle + 90) * (150-31) / 180
//    // 簡化後: Duty = 31 + (angle + 90) * 119 / 180
//    // 使用 long 避免計算溢位
//    int duty = 31 + (int)((long)(angle + 90) * 119 / 180);
//    set_servo_duty(duty);
//}

// VR (0-1023) 轉 馬達角度 (-90 ~ 90)
//int VR_value_to_servo_angle(int value) {
//    // 映射: 0->-90, 1023->90
//    // 公式: -90 + (value * 180 / 1023)
//    return -90 + (int)((long)value * 180 / 1023);
//}

// VR (0-1023) 轉 LED 亮度 (0 ~ 100)
// 用於 Q3b 或呼吸燈
//int VR_value_to_LED_analog(int value) {
//    return (int)((long)value * 100 / 1023);
//}

// 這是為了讓你可以寫 delay(0.5); 這種直覺的寫法
//int delay(double sec) {
//    int ms = (int)(sec * 1000);
//    for(int i=0; i<ms; i++) {
//        __delay_ms(1);
//    }
//    return 1;
//}

// 讀取目前 LED 的狀態 (回傳整數)
//int get_LED() {
//    return LATD;
//}

// 設定 LED 顯示數值 (二進位)
// 例如 value=3 -> 0011 -> RD0, RD1 亮
//void set_LED(int value) {
//    LATD = value;
//}

// 分別設定 4 顆 LED (a對應RD0, d對應RD3)
// 輸入 1 亮，0 滅
//void set_LED_separately(int a, int b, int c, int d) {
//    LATDbits.LATD4 = a;
//    LATDbits.LATD5 = b;
//    LATDbits.LATD6 = c;
//    LATDbits.LATD7 = d;
//}

// 軟體 PWM 控制單顆 LED 亮度 (模擬類比)
// value: 亮度 0 ~ 100
// 注意：這是阻塞式的 (Blocking)，會占用 CPU 時間
//void set_LED_analog(int value) {
//    if(value > 100) value = 100;
//    if(value < 0) value = 0;
//    
//    // 執行一次快速的 PWM 週期
//    // 若要持續亮，需在外部迴圈呼叫
//    LATDbits.LATD0 = 1;
//    for(int i=0; i<value; i++) __delay_us(50);
//    
//    LATDbits.LATD0 = 0;
//    for(int i=0; i<(100-value); i++) __delay_us(50);
//}