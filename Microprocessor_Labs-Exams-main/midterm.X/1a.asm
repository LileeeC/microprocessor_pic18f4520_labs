List p=18f4520 
    #include<p18f4520.inc>
    CONFIG OSC = INTIO67
    CONFIG WDT = OFF
    org 0x00

    
    MOVLW 0x3F; '01001100'
    MOVWF 0x20
    
    MOVLW b'01111111' ; '01001100'
    ANDWF 0x20

    MOVLW 0xFF ; 
    MOVWF 0x21
    
    MOVLW b'01111111' 
    ANDWF 0x21
    
    MOVLW b'00000001' ;  bit 0
    ANDWF 0x21, W
    
    
    RRNCF WREG
    IORWF 0x00
    
    MOVLW b'00000010' ;  bit 1
    ANDWF 0x21, W
    
    RRNCF WREG
    RRNCF WREG
    RRNCF WREG
    
    IORWF 0x00
    
    MOVLW b'00000100' ;  bit 2
    ANDWF 0x21, W
    
    RLNCF WREG
    RLNCF WREG
    RLNCF WREG
    
    IORWF 0x00
    
    MOVLW b'00001000' ;  bit 3
    ANDWF 0x21, W
    
    RLNCF WREG
   
    IORWF 0x00
    
    MOVLW b'00010000' ;  bit 4
    ANDWF 0x21, W
    
    RRNCF WREG
   
    IORWF 0x00
    
    MOVLW b'00100000' ;  bit 5
    ANDWF 0x21, W
    
    RRNCF WREG
    RRNCF WREG
    RRNCF WREG
   
    IORWF 0x00
    
    MOVLW b'01000000' ;  bit 6
    ANDWF 0x21, W
    
    RLNCF WREG
    RLNCF WREG
    RLNCF WREG
   
    IORWF 0x00
    
    MOVLW b'10000000' ;  bit 7
    ANDWF 0x21, W
    
    RLNCF WREG

    IORWF 0x00
    
    MOVFF 0x00, 0x21

    RRNCF 0x21
    
    MOVF 0x20, W
    CPFSEQ 0x21
    GOTO ELSE_BLOCK
    
IF_BLOCK: ; is palindrome
	MOVLW 0xFF
	MOVWF 0x10
	GOTO END_IF
ELSE_BLOCK:
	MOVLW 0x00
	MOVWF 0x10
END_IF:
    NOP
    







    end                   


