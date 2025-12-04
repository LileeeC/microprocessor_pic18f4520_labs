List p=18f4520 
    #include<p18f4520.inc>
    CONFIG OSC = INTIO67
    CONFIG WDT = OFF
    org 0x00
    
    
    MOVLW 0x92
    MOVWF 0x00
    
    MOVLW 0x76
    MOVWF 0x01
    
    MOVLW 0x19
    MOVWF 0x02
    
    MOVLW 0x04
    MOVWF 0x03
    
    MOVLW 0x78
    MOVWF 0x04
    
    MOVLW 0x13
    MOVWF 0x05
    
    MOVLW 0x61
    MOVWF 0x06
    
    MOVLW 0x67
    MOVWF 0x07
    
    MOVF 0x03, W
    ADDWF 0x07, W
    DAW
    MOVWF 0x24
    
    MOVF 0x02, W
    ADDWFC 0x06, W
    DAW
    MOVWF 0x23
    
    MOVF 0x01, W
    ADDWFC 0x05, W
    DAW
    MOVWF 0x22
    
    MOVF 0x00, W
    ADDWFC 0x04, W
    DAW
    MOVWF 0x21
    
    MOVLW 0x00
    ADDWFC 0x20
    

    MOVFF 0x20, 0x00
    MOVFF 0x21, 0x01
    MOVFF 0x22, 0x02
    MOVFF 0x23, 0x03
    MOVFF 0x24, 0x04
   
    
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
    
    
    end                   














