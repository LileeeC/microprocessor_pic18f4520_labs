List p=18f4520 
    #include<p18f4520.inc>
    CONFIG OSC = INTIO67
    CONFIG WDT = OFF
    org 0x00
    
initial:
    MOVLW 0xFA
    MOVWF 0x20
    
    MOVLW 0x9F
    MOVWF 0x21 ; N
    
    MOVLW 0x03
    MOVWF 0x22
    
    MOVLW 0x45
    MOVWF 0x23 ; x0 (xn)
    
    RCALL newtonSqrt
    GOTO finish
    
newtonSqrt:
    loop1:    
	MOVFF 0x20, 0x00
	MOVFF 0x21, 0x01 ; N as dividend

	MOVFF 0x22, 0x02
	MOVFF 0x23, 0x03 ; xn as divisor

	RCALL division ; N / xn

	MOVF 0x23, W ; xn low
	ADDWF 0x11 
	MOVF 0x22, W ; xn high
	ADDWFC 0x10 ; xn + N / xn

	MOVFF 0x10, 0x00
	MOVFF 0x11, 0x01 ; xn + N / xn as dividend

	MOVLW 0x00
	MOVWF 0x02
	MOVLW 0x02
	MOVWF 0x03 ; 2 as divisor

	RCALL division ; (xn + N / xn) / 2

	; if xn == xn + 1 -> stop

	MOVF 0x11, W
	CPFSEQ 0x23
	GOTO NOT_EQUAL

	MOVF 0x10, W
	CPFSEQ 0x22
	GOTO NOT_EQUAL

	EQUAL:
	    GOTO break1

	NOT_EQUAL:
	    MOVFF 0x11, 0x23
	    MOVFF 0x10, 0x22 ; xn = xn + 1

	GOTO loop1
    
    break1:
    MOVFF 0x23, 0x25
    MOVFF 0x22, 0x24
    
    RETURN
    
    
division:
    CLRF 0x10
    CLRF 0x11
    loop:
	MOVF 0x03, W
	SUBWF 0x01

	MOVF 0x02, W ; dividend - divisor
	SUBWFB 0x00

	BTFSS STATUS, C ; return if dividend < 0
	GOTO break

	MOVLW 0x01
	ADDWF 0x11
	MOVLW 0x00
	ADDWFC 0x10

	GOTO loop
	
    break:
	MOVF 0x03, W
	ADDWF 0x01

	MOVF 0x02, W
	ADDWFC 0x00

	MOVFF 0x01, 0x13
	MOVFF 0x00, 0x12
	
	RETURN

finish:
NOP
    end





