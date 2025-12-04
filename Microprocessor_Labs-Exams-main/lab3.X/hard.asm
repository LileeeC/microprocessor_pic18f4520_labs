List p=18f4520 
    #include<p18f4520.inc>
    CONFIG OSC = INTIO67
    CONFIG WDT = OFF
    org 0x00
    
    
    CLRF 0x00
    CLRF 0x01
    CLRF 0x02
    CLRF 0x10
    
    MOVLW 0xFE
    MOVWF 0x00
    
    MOVLW 0xFC
    MOVWF 0x01
    
    MOVLW b'00000001'
    MOVWF 0x10 ; mask
    
    loop:
	MOVF 0x10, W
	ANDWF 0x01, W
	
	BZ NO_ADD
	
	MOVF 0x00, W
	ADDWF 0x02
	
	NO_ADD:
	 
	RLNCF 0x10
	RLNCF 0x00
	MOVLW b'11111110'
	ANDWF 0x00
	
    BTFSS 0x10, 0
    GOTO loop
    NOP
    
    end











