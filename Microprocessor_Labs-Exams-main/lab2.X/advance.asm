List p=18f4520 
    #include<p18f4520.inc>
    CONFIG OSC = INTIO67
    CONFIG WDT = OFF
    org 0x00
    
    MOVLB 0x02
    
    MOVLW 0x00
    MOVWF 0x00, 1 ; Sequence A
    
    MOVLW 0x33
    MOVWF 0x01, 1
    
    MOVLW 0x58
    MOVWF 0x02, 1
    
    MOVLW 0x7A
    MOVWF 0x03, 1
    
    MOVLW 0xC4
    MOVWF 0x04, 1
    
    MOVLW 0xF0
    MOVWF 0x05, 1
    
    MOVLW 0x09
    MOVWF 0x10, 1 ; Sequence B
    
    MOVLW 0x58
    MOVWF 0x11, 1
    
    MOVLW 0x6E
    MOVWF 0x12, 1
    
    MOVLW 0xB8
    MOVWF 0x13, 1
    
    MOVLW 0xDD
    MOVWF 0x14, 1
    
    LFSR 0, 0x200 ; pointA
    LFSR 1, 0x210 ; pointB
    
    LFSR 2, 0x220 ; result
    
    loop:
	MOVLW 0x06
	SUBWF FSR0L, W
	BZ IF_BLOCK
	
	MOVLW 0x15
	SUBWF FSR1L, W
	BZ ELSE_BLOCK
	
	MOVF INDF1, W
	CPFSGT INDF0
	GOTO ELSE_BLOCK 
	
	
	
	IF_BLOCK: ; pointA > pointB or pointA empty
	    
	    MOVFF POSTINC1, POSTINC2 ; put B in res
	    GOTO END_IF
	
	ELSE_BLOCK: ; pointA <= pointB or pointB empty
	    MOVFF POSTINC0, POSTINC2 ; put A in res
    
	END_IF:
	
	
	MOVLW 0x2A
	CPFSGT FSR2L
	GOTO loop
    
    end


