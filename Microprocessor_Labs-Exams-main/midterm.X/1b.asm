List p=18f4520 
    #include<p18f4520.inc>
    CONFIG OSC = INTIO67
    CONFIG WDT = OFF
    org 0x00

    
    MOVLW 0x0C
    MOVWF 0x00
    
    MOVLW 0x22
    MOVWF 0x01
    
    MOVLW 0x22
    MOVWF 0x02
    
    MOVLW 0xC0
    MOVWF 0x03
    
    MOVLW 0xFF
    MOVWF 0x20
    
    MOVFF 0x00, 0x10
    
    MOVFF 0x01, 0x11
    MOVLW b'11110000'
    ANDWF 0x11
    
    MOVF 0x11, W
    IORWF 0x10
    
    MOVF 0x03, W
    CPFSEQ 0x10
    GOTO NOT_PALIN
    
    MOVFF 0x01, 0x11
    MOVLW b'00001111'
    ANDWF 0x11
    
    MOVFF 0x02, 0x12
    MOVLW b'00001111'
    ANDWF 0x12
    
    MOVF 0x12, W
    CPFSEQ 0x11
    GOTO NOT_PALIN
    
    
    GOTO finish
NOT_PALIN:
    MOVLW 0x00
    MOVWF 0x20
    
finish:
    MOVFF 0x20, 0x10
    NOP
    end                   








