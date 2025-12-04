List p=18f4520 
    #include<p18f4520.inc>
    CONFIG OSC = INTIO67
    CONFIG WDT = OFF
    org 0x00

    MOVLB 0x01
    
    MOVLW 0x8E
    MOVWF 0x20, 1
    
    MOVLW 0x37
    MOVWF 0x21, 1
    
    LFSR 0, 0x122 ;File Select Register FSR0 = addr
    
    loop:
	; load [addr - 1] to 0x100 (val1)
	MOVLW 0x01
	SUBWF FSR0L, W ; WREG = addr - 1	
	MOVWF FSR1L
	MOVF FSR0H, W
	MOVWF FSR1H
	MOVFF INDF1, 0x100
	
	; load [addr - 2] to 0x101 (val2)
	MOVLW 0x02
	SUBWF FSR0L, W
	MOVWF FSR1L
	MOVF FSR0H, W
	MOVWF FSR1H
	MOVFF INDF1, 0x101
	
	BTFSC FSR0L, 0
	GOTO ELSE_BLOCK
	
	IF_BLOCK: ; even -> [addr] = [0x101] + [0x100]
	    MOVF 0x00, W, 1
	    ADDWF 0x01, W, 1
	    MOVFF WREG,INDF0 
	    GOTO END_IF
	ELSE_BLOCK: ; odd ->: [addr] = [0x101] - [0x100]
	    MOVF 0x00, W, 1
	    SUBWF 0x01, W, 1
	    MOVFF WREG, INDF0
	
	END_IF:
	INCF FSR0L
	MOVLW 0x26
	CPFSGT FSR0L
	GOTO loop	
	
    end


