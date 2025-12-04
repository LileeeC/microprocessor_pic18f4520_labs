List p=18f4520 
    #include<p18f4520.inc>
    CONFIG OSC = INTIO67
    CONFIG WDT = OFF
    org 0x00
    
    MOVLB 0x02
    
    MOVLW 0x3B
    MOVWF 0x10 ; List 1
    
    MOVLW 0xF7
    MOVWF 0x11
    
    MOVLW 0xA2
    MOVWF 0x12
    
    MOVLW 0x50
    MOVWF 0x20; List 2
    
    MOVLW 0xA2
    MOVWF 0x21
    
    MOVLW 0xCB
    MOVWF 0x22
    
    MOVLW 0xA2
    MOVWF 0x30; List 3
    
    MOVLW 0xE9
    MOVWF 0x31
    
    MOVLW 0x13
    MOVWF 0x32
    
    MOVFF 0x10, 0x00
    MOVFF 0x11, 0x01
    MOVFF 0x12, 0x02
    
    MOVFF 0x20, 0x03
    MOVFF 0x21, 0x04
    MOVFF 0x22, 0x05
    
    MOVFF 0x30, 0x06
    MOVFF 0x31, 0x07
    MOVFF 0x32, 0x08

    LFSR 2, 0x008 ; j

	    loop1:
		LFSR 0, 0x000 ; i 
		LFSR 1, 0x001 ; i + 1

		loop2:
		    MOVF INDF1, W 
		    CPFSGT INDF0
		    GOTO ELSE_BLOCK_3

		    IF_BLOCK_3: ; if [i] > [i + 1]
			MOVF INDF0, W ; swap
			MOVFF INDF1, INDF0
			MOVWF INDF1
			GOTO END_IF_3

		    ELSE_BLOCK_3: 

		    END_IF_3:

		    INCF FSR0L ; fix: how to ++ a 12 bit register
		    INCF FSR1L ; fix: how to ++ a 12 bit register

		    MOVF FSR2L, W
		    CPFSGT FSR1L
		    GOTO loop2

		    DECF   FSR2L, F       ; ?? -- 

		BTFSC STATUS, N
		GOTO break
		GOTO loop1
    break:
    NOP
    end





