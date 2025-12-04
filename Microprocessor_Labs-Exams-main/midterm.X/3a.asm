List p=18f4520 
    #include<p18f4520.inc>
    CONFIG OSC = INTIO67
    CONFIG WDT = OFF
    org 0x00

    MOVLW 0x05
    MOVWF 0x00
    
    MOVLW 0x05
    MOVWF 0x01
    
    MOVLW 0x05
    MOVWF 0x02
    
    MOVLW 0x05
    MOVWF 0x03
    
    MOVLW 0x05
    MOVWF 0x04
    
    MOVLW 0x0A
    MOVWF 0x05; target
    
    CLRF 0x10; res
    
    LFSR 0, 0x000; slowptr, i
    LFSR 1, 0x001; fastptr, j
   
loopi:
    
    loopj:
    
	MOVF INDF0, W
	ADDWF INDF1, W
	
	CPFSEQ 0x05
	GOTO NOT_EQ_TARGET
	
    IF_EQ_TARGET:
	INCF 0x10
    NOT_EQ_TARGET:
    
	INCF FSR1L; ;j++

	MOVLW 0x04
	CPFSGT FSR1L
    GOTO loopj
    
    INCF FSR0L;i++
    
    MOVFF FSR0L, FSR1L
    INCF FSR1L ; j = i+1
    
    MOVLW 0x03
    CPFSGT FSR0L
GOTO loopi
    
    NOP
    end                   








