List p=18f4520 
    #include<p18f4520.inc>
    CONFIG OSC = INTIO67
    CONFIG WDT = OFF
    org 0x00

    CLRF TRISA
    MOVLW 0xA6
    MOVWF TRISA
    
    MOVLW b'00000001' ;  bit 0
    ANDWF TRISA, W
    
    RRNCF WREG
    IORWF 0x00
    
    MOVLW b'00000010' ;  bit 1
    ANDWF TRISA, W
    
    RRNCF WREG
    RRNCF WREG
    RRNCF WREG
    
    IORWF 0x00
    
    MOVLW b'00000100' ;  bit 2
    ANDWF TRISA, W
    
    RLNCF WREG
    RLNCF WREG
    RLNCF WREG
    
    IORWF 0x00
    
    MOVLW b'00001000' ;  bit 3
    ANDWF TRISA, W
    
    RLNCF WREG
   
    IORWF 0x00
    
    MOVLW b'00010000' ;  bit 4
    ANDWF TRISA, W
    
    RRNCF WREG
   
    IORWF 0x00
    
    MOVLW b'00100000' ;  bit 5
    ANDWF TRISA, W
    
    RRNCF WREG
    RRNCF WREG
    RRNCF WREG
   
    IORWF 0x00
    
    MOVLW b'01000000' ;  bit 6
    ANDWF TRISA, W
    
    RLNCF WREG
    RLNCF WREG
    RLNCF WREG
   
    IORWF 0x00
    
    MOVLW b'10000000' ;  bit 7
    ANDWF TRISA, W
    
    RLNCF WREG

    IORWF 0x00
    
    MOVFF 0x00, TRISA
    
    end





