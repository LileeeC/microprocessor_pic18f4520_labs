List p=18f4520 
    #include<p18f4520.inc>
    CONFIG OSC = INTIO67
    CONFIG WDT = OFF
    org 0x00
initial:
    MOVLW 0xFA
    MOVWF 0x00

    MOVLW 0x9F
    MOVWF 0x01

    MOVLW 0x03
    MOVWF 0x02

    MOVLW 0x45
    MOVWF 0x03

    CLRF 0x10
    CLRF 0x11

    RCALL division
    ; add divisor back and put to remainder
    GOTO finish
division:
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


