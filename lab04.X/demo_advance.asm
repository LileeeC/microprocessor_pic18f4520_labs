; unsigned 16-bit integer division
List p=18f4520 
    #include<p18f4520.inc>
    CONFIG OSC = INTIO67
    CONFIG WDT = OFF
    org 0x00
    
    DD_high EQU 0x00
    DD_low EQU 0x01 ; Dividend 0x000/0x001
    DS_high EQU 0x02
    DS_low EQU 0x03 ; Divisor 0x002/0x003
    Q_high EQU 0x10
    Q_low EQU 0x11 ; Quotient 0x010/0x011
    R_high EQU 0x12
    R_low EQU 0x13 ; Remainder 0x012/0x013
    CNT EQU 0x020 ; count 16 times
 
    MOVLW 0xFA
    MOVWF DD_high
    MOVLW 0x9F
    MOVWF DD_low
    MOVLW 0x03
    MOVWF DS_high
    MOVLW 0x45
    MOVWF DS_low
    RCALL initial
    RCALL division
    finish:
	GOTO finish
    
    initial:
	MOVLW 0x10
	MOVWF CNT
	BCF STATUS, C
	CLRF Q_high
	CLRF Q_low
	CLRF R_high
	CLRF R_low
	RETURN
	
    division:
	loop:
	    BCF STATUS, C ; C=0
	    ; Dividend << 1
	    RLCF DD_low, f ;rotate left through carry -> carry = DD_low's bit7
	    RLCF DD_high, f ; carry = DD's bit15
	    ; Remainder << 1
	    RLCF R_low, f
	    RLCF R_high, f ; R = R<<1 + DD's bit15
	    
	    ; Quotient << 1
	    BCF STATUS, C ;the carry isn't related to quotient
	    RLCF Q_low, f
	    RLCF Q_high, f
	    
	    ; try R = R - Divisor to see who's greater
	    MOVF DS_low, W
	    SUBWF R_low, f ; f = f-W
	    MOVF DS_high, W
	    SUBWFB R_high, f ; f = f-W-C
	    
	    BNC borrow ; Branch if No Carry (C=0)
	    ; C=0, borrow (R < divisor) -> Q=0 -> sub fail, add back divisor
	    ; C=1, no borrow (means R >= divisor) -> Q=1
	    
	    BSF Q_low, 0 ; set 0th bit = 1
	    BRA loop_cont
	    
	borrow: ; C=0
	    MOVF  DS_low, W
	    ADDWF R_low, f
	    MOVF  DS_high, W
	    ADDWFC R_high, f ; f = W+f+C
	    
	loop_cont:
	    DECFSZ CNT, f
	    GOTO loop
	    RETURN
END