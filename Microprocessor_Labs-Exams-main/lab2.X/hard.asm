List p=18f4520 
    #include<p18f4520.inc>
    CONFIG OSC = INTIO67
    CONFIG WDT = OFF
    org 0x00

    MOVLB 0x03

    MOVLW 0xC4
    MOVWF 0x00, 1

    MOVLW 0xBB
    MOVWF 0x01, 1

    MOVLW 0xBB
    MOVWF 0x02, 1

    MOVLW 0x00
    MOVWF 0x03, 1

    MOVLW 0x4C
    MOVWF 0x04, 1

    MOVLW 0x8B
    MOVWF 0x05, 1

    MOVLW 0xBB
    MOVWF 0x06, 1 

    MOVLW 0x00
    MOVWF 0x07, 1

    LFSR 0, 0x300 ; slow ptr
    LFSR 1, 0x301 ; fast ptr
    LFSR 2, 0x320 ; res ptr (unsorted)

    CLRF 0x09, 1 ; pair_cnt

    loopo: ; move slow
	loop: ; move fast

	    RLNCF INDF1, W
	    RLNCF WREG
	    RLNCF WREG
	    RLNCF WREG
	    
	    
	    
	    CPFSEQ INDF0
	    GOTO ELSE_BLOCK
	    
	    MOVLW 0x10
	    ADDWF FSR0L
	    ADDWF FSR1L
	    
	    MOVFF INDF0, 0x30A
	    MOVFF INDF1, 0x30B
	    
	    MOVLW 0x10
	    SUBWF FSR0L
	    SUBWF FSR1L
	    
	    MOVF 0x0A, W, 1
	    BNZ ELSE_BLOCK
	    
	    MOVF 0x0B, W, 1
	    BNZ ELSE_BLOCK

	    IF_BLOCK: ; is reverse(0x2B 0xB2) and not paired
		INCF    0x09, f, 1 ; pair_cnt++
		
		MOVLW 0x10
		ADDWF FSR0L
		ADDWF FSR1L
		
		INCF INDF0
		INCF INDF1
		
		MOVLW 0x10
		SUBWF FSR0L
		SUBWF FSR1L
	
		MOVF INDF1, W
		CPFSGT INDF0
		GOTO ELSE_BLOCK_1 

		IF_BLOCK_1: ; if slow > fast
		    MOVFF INDF1, POSTINC2 ; put fast in res array
		    GOTO END_IF_1

		ELSE_BLOCK_1: ; else (slow >= fast)
		    MOVFF INDF0, POSTINC2 ; put slow in res array

		END_IF_1:
	
		GOTO break_loop

	    ELSE_BLOCK: ; not reverse
		INCF FSR1L ; fix: how to ++ a 12 bit register
		
		MOVLW 0x07
		CPFSGT FSR1L
		GOTO loop
	    break_loop:
    
	    ; slow++
	    INCF FSR0L ; fix: how to ++ a 12 bit register
	    MOVFF FSR0L, FSR1L ; can specify by 12bits but cant move 12bits (MOVFF FSR0, FSR1 is wrong)
	    INCF FSR1L ; fix: how to ++ a 12 bit register
	    
	MOVLW 0x07
	CPFSGT FSR0L
	GOTO loopo
	
	MOVLW 0x04
	CPFSEQ 0x09, 1
	GOTO ELSE_BLOCK_2 

	IF_BLOCK_2: ; if pair_cnt == 4
	    ;sort res
	    LFSR 2, 0x323 ; j

	    loop1:
		LFSR 0, 0x320 ; i 
		LFSR 1, 0x321 ; i + 1

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



		MOVLW 0x20
		CPFSLT FSR2L
		GOTO loop1


	    ;PUT LOW TO HIGH

	    RLNCF 0x20, W, 1
	    RLNCF WREG
	    RLNCF WREG
	    RLNCF WREG
	    MOVWF 0x27, 1

	    RLNCF 0x21, W, 1
	    RLNCF WREG
	    RLNCF WREG
	    RLNCF WREG
	    MOVWF 0x26, 1

	    RLNCF 0x22, W, 1
	    RLNCF WREG
	    RLNCF WREG
	    RLNCF WREG
	    MOVWF 0x25, 1

	    RLNCF 0x23, W, 1
	    RLNCF WREG
	    RLNCF WREG
	    RLNCF WREG
	    MOVWF 0x24, 1

	    GOTO END_IF_2

	ELSE_BLOCK_2:
		MOVLW 0xFF
		MOVWF 0x20, 1
		MOVLW 0xFF
		MOVWF 0x21, 1
		MOVLW 0xFF
		MOVWF 0x22, 1
		MOVLW 0xFF
		MOVWF 0x23, 1
		MOVLW 0xFF
		MOVWF 0x24, 1
		MOVLW 0xFF
		MOVWF 0x25, 1
		MOVLW 0xFF
		MOVWF 0x26, 1
		MOVLW 0xFF
		MOVWF 0x27, 1
	END_IF_2:
	
    
    
    
    
    
    end


