List p=18f4520 
    #include<p18f4520.inc>
    CONFIG OSC = INTIO67
    CONFIG WDT = OFF
    org 0x00

    And_Mul macro xh, xl, yh, yl
        MOVLW xh
        MOVWF 0x02
        MOVLW xl
        MOVWF 0x03
        MOVLW yh
        MOVWF 0x04
        MOVLW yl
        MOVWF 0x05
    
        MOVF 0x02, W ; and x, y
        ANDWF 0x04, W
        MOVWF 0x00

        MOVF 0x03, W
        ANDWF 0x05, W
        MOVWF 0x01

        MOVF 0x00, W ; mul 0x00, 0x01
        MULWF 0x01
        MOVFF PRODL, 0x11
        MOVFF PRODH, 0x10
        
        endm
    

    And_Mul 0x50, 0x6F, 0x3A, 0xBC
    
    end


