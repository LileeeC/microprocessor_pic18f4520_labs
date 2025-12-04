List p=18f4520 
    #include<p18f4520.inc>
    CONFIG OSC = INTIO67
    CONFIG WDT = OFF
    org 0x00
    
    MOVLW b'11111111'
    MOVWF 0x00

    MOVLW 0x00
    MOVWF 0x10
    
    MOVF 0x00, W
    BZ skipz

    BTFSC 0x00, 7
    GOTO skip

    loop:
        INCF 0x10
        RLNCF 0x00
        BTFSS 0x00, 7
        GOTO loop
    GOTO skip
    
    
    skipz:
    MOVLW 0x08
    MOVWF 0x10
    skip:

    end

