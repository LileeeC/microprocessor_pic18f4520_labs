LIST p=18f4520
#include<p18f4520.inc>

    CONFIG OSC = INTIO67 ; Set internal oscillator to 1 MHz
    CONFIG WDT = OFF     ; Disable Watchdog Timer
    CONFIG LVP = OFF     ; Disable Low Voltage Programming

    L1 EQU 0x14         ; Define L1 memory location
    L2 EQU 0x15         ; Define L2 memory location
    org 0x00            ; Set program start address to 0x00

; instruction frequency = 1 MHz / 4 = 0.25 MHz
; instruction time = 1/0.25 = 4 ?s
; Total_cycles = 2 + (2 + 8 * num1 + 3) * num2 cycles
; num1 = 111, num2 = 70, Total_cycles = 62512 cycles
; Total_delay ~= Total_cycles * instruction time = 0.25 s
DELAY macro num1, num2
    local LOOP1         ; Inner loop
    local LOOP2         ; Outer loop
    
    ; 2 cycles
    MOVLW num2          ; Load num2 into WREG
    MOVWF L2            ; Store WREG value into L2
    
    ; Total_cycles for LOOP2 = 2 cycles
    LOOP2:
    MOVLW num1          
    MOVWF L1  
    
    ; Total_cycles for LOOP1 = 8 cycles
    LOOP1:
    NOP                 ; busy waiting
    NOP
    NOP
    NOP
    NOP
    DECFSZ L1, 1        
    BRA LOOP1           ; BRA instruction spends 2 cycles
    
    ; 3 cycles
    DECFSZ L2, 1        ; Decrement L2, skip if zero
    BRA LOOP2           
endm

start:
int:
; let pin can receive digital signal
    MOVLW 0x0f          ; Set ADCON1 register for digital mode
    MOVWF ADCON1        ; Store WREG value into ADCON1 register
    CLRF PORTB          ; Clear PORTB
    BSF TRISB, 0        ; Set RB0 as input (TRISB = 0000 0001)
    CLRF LATA           ; Clear LATA
    BCF TRISA, 0        ; Set RA0 as output (TRISA = 0000 0000)
    BCF TRISA, 1
    BCF TRISA, 2
    
; Button check
initial:        
    CLRF LATA
    DELAY d'111', d'70' 
    BTFSC PORTB, 0      ; Check if PORTB bit 0 is low (button pressed)
    BRA initial  ; If button is not pressed, branch back to check_process
    BRA state1
    
state1:
    ; 0.5
    CLRF LATA
    BTG LATA, 2
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA state2	    ; if button is pressed
    DELAY d'111', d'35'
    BTFSS PORTB, 0 
    BRA state2	    ; if button is pressed
    DELAY d'111', d'35'
    BTFSS PORTB, 0 
    BRA state2	    ; if button is pressed
    DELAY d'111', d'35'
    BTFSS PORTB, 0 
    BRA state2
    
    CLRF LATA
    BTG LATA,1 
    DELAY d'111', d'35' 
    BTFSS PORTB, 0 
    BRA state2	    ; if button is pressed
    DELAY d'111', d'35' 
    BTFSS PORTB, 0 
    BRA state2	    ; if button is pressed
    DELAY d'111', d'35' 
    BTFSS PORTB, 0 
    BRA state2	    ; if button is pressed
    DELAY d'111', d'35' 
    BTFSS PORTB, 0 
    BRA state2
    
    CLRF LATA
    BTG LATA, 0
    DELAY d'111', d'35' 
    BTFSS PORTB, 0 
    BRA state2	    ; if button is pressed
    DELAY d'111', d'35' 
    BTFSS PORTB, 0 
    BRA state2	    ; if button is pressed
    DELAY d'111', d'35' 
    BTFSS PORTB, 0 
    BRA state2	    ; if button is pressed
    DELAY d'111', d'35' 
    BTFSS PORTB, 0 
    BRA state2
    
    BRA state1
    
state2:
    ; 1
    CLRF LATA
    BTG LATA, 2
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    
    ; 1
    CLRF LATA
    BTG LATA, 1
    BTG LATA, 2
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    
    BRA flash

flash:    
    ; 0.5
    CLRF LATA
    BTG LATA, 0
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    
    ; 1
    CLRF LATA
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    
    ; 0.5
    CLRF LATA
    BTG LATA, 0
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    
    ; 1
    CLRF LATA
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    
    ; 0.5
    CLRF LATA
    BTG LATA, 0
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    DELAY d'111', d'35'    ; 0.125
    BTFSS PORTB, 0 
    BRA initial
    
    BRA state2
    
end