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
#define _XTAL_FREQ 4000000
#define STR_MAX 100
#define VR_MAX ((1 << 10) - 1)
// #define delay(t) __delay_ms(t * 1000);

//---------------- Functions --------------------

// ===== UART functions =====
void putch(char data);
void ClearBuffer();
void MyusartRead();
int GetString(char *str);


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

// ===== Utility =====
int delay(double sec);

//---------------- Global Variebles --------------------

char buffer[STR_MAX];
int buffer_size = 0;
bool btn_interr = false;

int pre_radc_val = 0;

// ---------------- Settings --------------------

void TMR1_Initialize(void){
    PIR1bits.TMR1IF = 0;   // Clear the Timer1 interrupt flag
    IPR1bits.TMR1IP = 0;   // Set Timer1 interrupt priority to low (1 = high, 0 = low)
    PIE1bits.TMR1IE = 1;   // Enable Timer1 interrupt (allow Timer1 overflow to trigger an ISR)

    T1CONbits.T1CKPS = 0b11;  // Timer1 prescaler = 1:8 
    T1CONbits.TMR1CS = 0b0;   // Timer1 clock source = internal clock (Fosc/4)
    
    TMR1 = 53035; // Interrupt Every 0.1 sec (When Fosc = 4M)
    T1CONbits.TMR1ON = 1; //open timer1
}

void TMR2_Initialize(void){

}

void TMR3_Initialize(void){

}

void ADC_Initialize(void){
    // Configure ADC
    TRISAbits.RA0 = 1;         // Set RA0 as input port
    ADCON0bits.CHS = 0b0000;   // Select AN0 channel
    ADCON1bits.VCFG0 = 0;      // Vref+ = Vdd
    ADCON1bits.VCFG1 = 0;      // Vref- = Vss
    ADCON2bits.ADCS = 0b000;   // ADC clock Fosc/2
    ADCON2bits.ACQT = 0b001;   // 2Tad acquisition time
    ADCON0bits.ADON = 1;       // Enable ADC
    ADCON2bits.ADFM = 1;       // Right justified
}

void PWM_Initialize(void){
    // Configure servo (PWM)
    T2CONbits.TMR2ON = 0b1;      // Timer2 on
    T2CONbits.T2CKPS = 0b11;     // Prescaler 16
    CCP1CONbits.CCP1M = 0b1100;  // PWM mode
    PR2 = 0x9b;                  // Set PWM period

    TRISCbits.TRISC2 = 0;        // PORTC as output for servo
    LATC = 0;                    // Clear PORTC
}

void IO_Initialize(void){
    // Configure I/O ports
    TRISD &= 0x0F;  // Set RD4-RD7 as outputs for LED
    TRISB = 1;      // RB0 as input for button
    LATD &= 0x0F;   // Clear RD4-RD7
}

void Interrupt_Initialize(void){
    // Configure interrupts
    INTCONbits.INT0IF = 0;  // Clear INT0 flag
    INTCONbits.INT0IE = 1;  // Enable INT0 interrupt
    PIE1bits.ADIE = 1;      // Enable ADC interrupt
    PIR1bits.ADIF = 0;      // Clear ADC flag
    INTCONbits.PEIE = 1;    // Enable peripheral interrupt
    INTCONbits.GIE = 1;     // Enable global interrupt
    RCONbits.IPEN = 1;      // enable Interrupt Priority mode
    INTCONbits.GIEH = 1;    // enable high priority interrupt
    INTCONbits.GIEL = 1;    // disable low priority interrupt
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

    // Setting Baud rate
    // Baud rate = 1200 (Look up table)
    TXSTAbits.SYNC = 0;     // Synchronus or Asynchronus
    BAUDCONbits.BRG16 = 0;  // 16 bits or 8 bits
    TXSTAbits.BRGH = 0;     // High Baud Rate Select bit
    SPBRG = 51;             // Control the period

    // Serial enable
    RCSTAbits.SPEN = 1;  // Enable asynchronus serial port (must be set to 1)
    PIR1bits.TXIF = 0;   // Set when TXREG is empty
    PIR1bits.RCIF = 0;   // Will set when reception is complete
    TXSTAbits.TXEN = 1;  // Enable transmission
    RCSTAbits.CREN = 1;  // Continuous receive enable bit, will be cleared when error occured
    PIE1bits.TXIE = 0;   // Wanna use Interrupt (Transmit)
    IPR1bits.TXIP = 0;   // Interrupt Priority bit
    PIE1bits.RCIE = 1;   // Wanna use Interrupt (Receive)
    IPR1bits.RCIP = 0;   // Interrupt Priority bit
    /* Transmitter (output)
     TSR   : Current Data
     TXREG : Next Data
     TXSTAbits.TRMT will set when TSR is empty
    */
    /* Reiceiver (input)
     RSR   : Current Data
     RCREG : Correct Data (have been processed) : read data by reading the RCREG Register
    */

}
void Initialize(void) {
    OSCCONbits.IRCF = 0b110;  // Osc = 4 MHz
    ADCON1bits.PCFG = 0b1110;  // AN0 as analog input

    //TMR1_Initialize();
    //TMR2_Initialize();
    //TMR3_Initialize();
    ADC_Initialize();
    //PWM_Initialize();
    IO_Initialize();
    Interrupt_Initialize();
    UART_Initialize();

    // Start ADC conversion
    ADCON0bits.GO = 1;
}

// ---------------- OOP --------------------

// ---------------- Uart --------------------

void putch(char data) {  // Output on Terminal
    if (data == '\n' || data == '\r') {
        while (!TXSTAbits.TRMT);
        TXREG = '\r';
        while (!TXSTAbits.TRMT);
        TXREG = '\n';
    } else {
        while (!TXSTAbits.TRMT);
        TXREG = data;
    }
}

void ClearBuffer() {
    for (int i = 0; i < STR_MAX; i++)
        buffer[i] = '\0';
    buffer_size = 0;
}

void MyusartRead() {
    char data = RCREG;
    if (!isprint(data) && data != '\r') return;
    buffer[buffer_size++] = data;
    putch(data);
}

int GetString(char *str) {
    if (buffer[buffer_size - 1] == '\r') {
        buffer[--buffer_size] = '\0';
        strcpy(str, buffer);
        ClearBuffer();
        return 1;
    } else {
        str[0] = '\0';
        return 0;
    }
}
// ----------------------------------------------------
int get_LED() {
    return (LATD >> 4);
}

void set_LED(int value) {
    LATD = (value << 4);
}

void set_LED_separately(int a, int b, int c, int d) {
    LATD = (a << 7) + (b << 6) + (c << 5) + (d << 4);
}

void set_LED_analog(int value) {
    CCPR1L = (value >> 2);
    CCP1CONbits.DC1B = (value & 0b11);
}

int current_servo_angle = 0;
int get_servo_angle() {
    // double duty_cycle = ((CCPR1L << 2) + CCP1CONbits.DC1B) * 4;
    // return (int)((duty_cycle - 500) / (2400 - 500) * 180 - 90);
    return current_servo_angle;
}

int set_servo_angle(int angle) {
    int current = (CCPR1L << 2) + CCP1CONbits.DC1B;
    int target = (int)((500 + (double)(angle + 90) / 180 * (2400 - 500)) / 8 / 4) * 8;  // angle to pwn
    btn_interr = false;
    while (current != target) {
        if (btn_interr) return -1;
        if (current < target)
            current++;
        else
            current--;

        CCPR1L = (current >> 2);
        CCP1CONbits.DC1B = (current & 0b11);
        __delay_ms(1);
    }
    current_servo_angle = angle;
    return 0;
}

int VR_value_to_servo_angle(int value) {
    return (int)(((double)value / VR_MAX * 180) - 90);
}

int VR_value_to_LED_analog(int value) {
    return value;
}

int delay(double sec) {
    btn_interr = false;
    for (int i = 0; i < sec * 1000 / 10; i++) {
        if (btn_interr) return -1;
        __delay_ms(10);
    }
    return 0;
}

// --------------- TODO ------------------

void button_pressed(void) {
    // Do sth when the button is pressed
    /* Example:
     * set_LED(get_LED() + 1);
     */


}

void variable_register_changed(int value) {  // value: 0 ~ 1023
    // Do sth when the variable register changes
    /* Example:
     * set_servo_angle(VR_value_to_servo_angle(value));
     * set_LED_analog(VR_value_to_LED_analog(value));
     * printf("%d\n", value); // print the variable register value on uart terminal
     */
    if(value < 85){
        if(pre_radc_val != 4){
            set_LED(4);
            printf("\b\b\b\b%04u", value); // ???????
            pre_radc_val = 4;
        }
    }
    else if(value < 170){
        if(pre_radc_val != 5){
            set_LED(5);
            printf("\b\b\b\b%04u", value); // ???????
            pre_radc_val = 5;
        }
    }
    else if(value < 256){
        if(pre_radc_val != 6){
            set_LED(6);
            printf("\b\b\b\b%04u", value); // ???????
            pre_radc_val = 6;
        }
    }
    else if(value < 341){
        if(pre_radc_val != 7){
            set_LED(7);
            printf("\b\b\b\b%04u", value); // ???????
            pre_radc_val = 7;
        }
    }
    else if(value < 426){
        if(pre_radc_val != 8){
            set_LED(8);
            printf("\b\b\b\b%04u", value); // ???????
            pre_radc_val = 8;
        }
    }
    else if(value < 512){
        if(pre_radc_val != 9){
            set_LED(9);
            printf("\b\b\b\b%04u", value); // ???????
            pre_radc_val = 9;
        }
    }
    else if(value < 597){
        if(pre_radc_val != 10){
            set_LED(10);
            printf("\b\b\b\b%04u", value); // ???????
            pre_radc_val = 10;
        }
    }
    else if(value < 682){
        if(pre_radc_val != 11){
            set_LED(11);
            printf("\b\b\b\b%04u", value); // ???????
            pre_radc_val = 11;
        }
    }
    else if(value < 767){
        if(pre_radc_val != 12){
            set_LED(12);
            printf("\b\b\b\b%04u", value); // ???????
            pre_radc_val = 12;
        }
    }
    else if(value < 852){
        if(pre_radc_val != 13){
            set_LED(13);
            printf("\b\b\b\b%04u", value); // ???????
            pre_radc_val = 13;
        }
    }
    else if(value < 938){
        if(pre_radc_val != 14){
            set_LED(14);
            printf("\b\b\b\b%04u", value); // ???????
            pre_radc_val = 14;
        }
    }
    else{
        if(pre_radc_val != 15){
            set_LED(15);
            printf("\b\b\b\b%04u", value); // ???????
            pre_radc_val = 15;
        }
    }
    return;

}

void keyboard_input(char *str) {  // get line from keyboard: this function will be called after you click enter
    // Do sth when typing on keyboard
    /* Example:
        if(strcmp(str, "mode1") == 0) {
            mode = 1;
        } else if(strcmp(str, "mode2") == 0) {
            mode = 2;
        }
     */


}

void timer1_interrupted(void) {  // Do sth when the timer is interrupted
    
}

// ---------------- Interrupt --------------------
void __interrupt(high_priority) H_ISR(void) {
    if (PIR1bits.ADIF) {  // Handle variable register interrupt
        int value = (ADRESH << 8) + ADRESL;
        variable_register_changed(value);
        PIR1bits.ADIF = 0;
        __delay_ms(5);
    }

    if (INTCONbits.INT0IF) {  // Handle button interrupt
        INTCONbits.INT0IE = 0;  
        button_pressed();
        __delay_ms(50);  // bouncing problem
        btn_interr = true;
        INTCONbits.INT0IF = 0;
        INTCONbits.INT0IE = 1;  
    }
}

void __interrupt(low_priority) Lo_ISR(void) {
    if (RCIF) {
        if (RCSTAbits.OERR) {
            CREN = 0;
            Nop();
            CREN = 1;
        }

        MyusartRead();
    }
    if(PIR1bits.TMR1IF){
        timer1_interrupted();
        TMR1 = 61629;
        PIR1bits.TMR1IF = 0;
    }

    // process other interrupt sources here, if required
    return;
}

void main() {
    Initialize();
    /* Usage:
     * set_servo_angle(-90); // input: -90 ~ 90, return value: -1 represents interrupt with button press, else 0
     * get_servo_angle(); // return value: -90 ~ 90
     *
     * set_LED(5); // 5 = 0b101, set LED1 and LED3 on, LED2 off
     * get_LED(); // return value: an integer, bit 0 -> LED1, bit 1 -> LED2, bit 2 -> LED3
     *
     * set_LED_separately(1, 1, 0); // set LED1 and LED2 on, LED3 off
     * set_LED_analog(512); // input: 0 ~ 1023, represent brightness. NOTICE: LED need to be plugged into the CCP1 pin.
     *
     * VR_value_to_servo_angle(1024); // return value: -90 ~ 90. Change the variable register value to servo angle
     * VR_value_to_LED_analog(1024); // return value: 0 ~ 1023. Change the variable register value to LED brightness
     *
     * delay(1); // delay 1 second, return value: -1 represents interrupt with button press, else 0
     *
     * printf(); // print on uart terminal
     */

    char str[STR_MAX];

    while (1) {
        // Do sth in main
        if (GetString(str)) keyboard_input(str);
        if (ADCON0bits.GO == 0) ADCON0bits.GO = 1;
    }
}