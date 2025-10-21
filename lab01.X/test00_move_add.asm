List p=18f4520
    #include<p18f4520.inc>
	CONFIG OSC = INTIO67
	CONFIG WDT = OFF
	org 0x00 ;starts from 0x00

	MOVLW 0x2b ;assign hexadecimal value to working register
	MOVWF 0x00 ;move data from working register to file register
	
	MOVLW D'15' ;decimal
	MOVWF 0x01

	MOVLW b'00000111' ;binary
	
	CLRF 0x00 ;clear register
	INCF 0x00 ;+1
	INCF 0x00
	DECF 0x00
	CLRF WREG
	
	MOVLW 0x12
	MOVWF 0x00
	MOVLW 0x31
	ADDWF 0x00, F ;add W to register "f"
	ADDWF 0x00, D
	
	MOVF 0x00, W ;move data from file register to W
	
	end