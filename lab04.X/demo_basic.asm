List p=18f4520 
    #include<p18f4520.inc>
    CONFIG OSC = INTIO67
    CONFIG WDT = OFF
    org 0x00

    And_Mul macro xh, xl, yh, yl
	MOVF xh, W
	ANDWF yh, W
	MOVWF 0x00
	
	MOVF xl, W
	ANDWF yl, W
	MOVWF 0x01
	
	MOVF 0x00, W
	MULWF 0x01
	
	MOVFF PRODH, 0x10
	MOVFF PRODL, 0x11
    endm
    
    MOVLW 0x50
    MOVWF 0x02
    MOVLW 0x6F
    MOVWF 0x03
    MOVLW 0x3A
    MOVWF 0x04
    MOVLW 0xBC
    MOVWF 0x05
    And_Mul 0x02, 0x03, 0x04, 0x05
    
    END