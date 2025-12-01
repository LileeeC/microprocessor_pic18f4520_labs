#include "p18f4520.inc"

; CONFIG1H
  CONFIG  OSC = INTIO67         ; Oscillator Selection bits (Internal oscillator block, port function on RA6 and RA7)
  CONFIG  FCMEN = OFF           ; Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
  CONFIG  IESO = OFF            ; Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

; CONFIG2L
  CONFIG  PWRT = OFF            ; Power-up Timer Enable bit (PWRT disabled)
  CONFIG  BOREN = SBORDIS       ; Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
  CONFIG  BORV = 3              ; Brown Out Reset Voltage bits (Minimum setting)

; CONFIG2H
  CONFIG  WDT = OFF             ; Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
  CONFIG  WDTPS = 32768         ; Watchdog Timer Postscale Select bits (1:32768)

; CONFIG3H
  CONFIG  CCP2MX = PORTC        ; CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
  CONFIG  PBADEN = ON           ; PORTB A/D Enable bit (PORTB<4:0> pins are configured as analog input channels on Reset)
  CONFIG  LPT1OSC = OFF         ; Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
  CONFIG  MCLRE = ON            ; MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

; CONFIG4L
  CONFIG  STVREN = ON           ; Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
  CONFIG  LVP = OFF             ; Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
  CONFIG  XINST = OFF           ; Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

; CONFIG5L
  CONFIG  CP0 = OFF             ; Code Protection bit (Block 0 (000800-001FFFh) not code-protected)
  CONFIG  CP1 = OFF             ; Code Protection bit (Block 1 (002000-003FFFh) not code-protected)
  CONFIG  CP2 = OFF             ; Code Protection bit (Block 2 (004000-005FFFh) not code-protected)
  CONFIG  CP3 = OFF             ; Code Protection bit (Block 3 (006000-007FFFh) not code-protected)

; CONFIG5H
  CONFIG  CPB = OFF             ; Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
  CONFIG  CPD = OFF             ; Data EEPROM Code Protection bit (Data EEPROM not code-protected)

; CONFIG6L
  CONFIG  WRT0 = OFF            ; Write Protection bit (Block 0 (000800-001FFFh) not write-protected)
  CONFIG  WRT1 = OFF            ; Write Protection bit (Block 1 (002000-003FFFh) not write-protected)
  CONFIG  WRT2 = OFF            ; Write Protection bit (Block 2 (004000-005FFFh) not write-protected)
  CONFIG  WRT3 = OFF            ; Write Protection bit (Block 3 (006000-007FFFh) not write-protected)

; CONFIG6H
  CONFIG  WRTC = OFF            ; Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
  CONFIG  WRTB = OFF            ; Boot Block Write Protection bit (Boot block (000000-0007FFh) not write-protected)
  CONFIG  WRTD = OFF            ; Data EEPROM Write Protection bit (Data EEPROM not write-protected)

; CONFIG7L
  CONFIG  EBTR0 = OFF           ; Table Read Protection bit (Block 0 (000800-001FFFh) not protected from table reads executed in other blocks)
  CONFIG  EBTR1 = OFF           ; Table Read Protection bit (Block 1 (002000-003FFFh) not protected from table reads executed in other blocks)
  CONFIG  EBTR2 = OFF           ; Table Read Protection bit (Block 2 (004000-005FFFh) not protected from table reads executed in other blocks)
  CONFIG  EBTR3 = OFF           ; Table Read Protection bit (Block 3 (006000-007FFFh) not protected from table reads executed in other blocks)

; CONFIG7H
  CONFIG  EBTRB = OFF           ; Boot Block Table Read Protection bit (Boot block (000000-0007FFh) not protected from table reads executed in other blocks)

    L1 EQU 0x14
    L2 EQU 0x15
    CNT EQU 0x20		; 加減去控制亮燈順序
    STA EQU 0x21		; 紀錄現在在 count up 還是 count down
    org 0x00
    
DELAY macro num1, num2 
    local LOOP1 
    local LOOP2
    MOVLW num2
    MOVWF L2
    LOOP2:
	MOVLW num1
	MOVWF L1
    LOOP1:
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	DECFSZ L1, 1
	BRA LOOP1
	DECFSZ L2, 1
	BRA LOOP2
endm

goto Initial			; 避免程式一開始就會執行到ISR這一段，要跳過。
ISR:				; Interrupt發生時，會跳到這裡執行。
    org 0x08			
    MOVLW 0x01
    XORWF STA, F		; 0 <-> 1
    MOVF STA, W
    BNZ CNT15			; check state flag, if != 0 (==1, count down) then set counter to 15
    BRA CNT0
CNT0:
    CLRF CNT			; will be count up
    BRA Show
CNT15:
    MOVLW 0x0F
    MOVWF CNT			; will be count down
Show:
    MOVF CNT, W
    SWAPF WREG, W		; swap high nibbles and low nibbles in wreg
    MOVWF LATD
    BCF INTCON, INT0IF
    RETFIE			; 離開ISR，回到原本程式執行的位址，同時會將GIE設為1，允許之後的interrupt能夠觸發
    
    
Initial:				
    MOVLW 0x0F
    MOVWF ADCON1		; 設定成要用數位的方式，Digitial I/O 
    
    CLRF TRISD			; PORTD 8 根腳位全部都是 Output
    CLRF LATD			; 現在輸出低電位 => LED 關燈
    CLRF CNT
    CLRF STA
    BSF TRISB,  0		; TRISB 的第 0 個 bit 設為 1 => RB0為input
    BCF RCON, IPEN
    BCF INTCON, INT0IF		; 先將Interrupt flag bit清空
    BSF INTCON, GIE		; 將Global interrupt enable bit打開
    BSF INTCON, INT0IE		; 將interrupt0 enable bit 打開 (INT0與RB0 pin腳位置相同) (白話：開啟 INT0 中斷功能)
    
main:
    MOVF CNT, W
    SWAPF WREG, W
    MOVWF LATD		; show at RD7~RD4
    
    DELAY d'111', d'125'	; 0.5s
    MOVF STA, W
    BNZ CountDown		; check state flag, if != 0 (==1) then do count dowm
    
CountUp:
    INCF CNT, F		; cnt++
    MOVF CNT, W
    ANDLW 0x0F
    MOVWF CNT
    BRA main
CountDown:
    DECF CNT, F	; cnt--
    MOVF CNT, W
    ANDLW 0x0F
    MOVWF CNT
    BRA main
end