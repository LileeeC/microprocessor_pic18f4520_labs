List p=18f4520 
    #include<p18f4520.inc>
    CONFIG OSC = INTIO67
    CONFIG WDT = OFF
    org 0x00

    CLRF TRISA
    MOVLW 0xA6
    MOVWF TRISA
    TMP EQU 0x00
    
    MOVLW b'00000001' ;mask for bit 0
    ANDWF TRISA, W ;W = TRISA & mask
    RRNCF WREG ;00000001 -> 10000000
    IORWF TMP ;f = f | W
    
    MOVLW b'00000010' ;mask for bit 1
    ANDWF TRISA, W
    RRNCF WREG
    RRNCF WREG
    RRNCF WREG
    IORWF TMP
    
    MOVLW b'00000100' ;mask for bit 2
    ANDWF TRISA, W
    RLNCF WREG
    RLNCF WREG
    RLNCF WREG
    IORWF TMP
    
    MOVLW b'00001000' ;mask for bit 3
    ANDWF TRISA, W
    RLNCF WREG
    IORWF TMP
    
    MOVLW b'00010000' ;mask for bit 4
    ANDWF TRISA, W
    RRNCF WREG
    IORWF TMP
    
    MOVLW b'00100000' ;mask for bit 5
    ANDWF TRISA, W
    RRNCF WREG
    RRNCF WREG
    RRNCF WREG
    IORWF TMP
    
    MOVLW b'01000000' ;mask for bit 6
    ANDWF TRISA, W
    RLNCF WREG
    RLNCF WREG
    RLNCF WREG
    IORWF TMP
    
    MOVLW b'10000000' ;mask for bit 7
    ANDWF TRISA, W
    RLNCF WREG
    IORWF TMP
    
    MOVFF TMP, TRISA ;put result in TRISA
end