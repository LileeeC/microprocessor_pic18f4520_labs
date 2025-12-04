List p=18f4520 
    #include<p18f4520.inc>
    CONFIG OSC = INTIO67
    CONFIG WDT = OFF
    org 0x00

    MOVLW 0x9A
    MOVWF 0x00
    
    MOVLW 0xBC
    MOVWF 0x01
    
    MOVLW 0x12
    MOVWF 0x10
    
    MOVLW 0x34
    MOVWF 0x11
    
    MOVF 0x11, W ; sub low
    SUBWF 0x01, W
    
    MOVWF 0x21
    
    BC NO_BORROW
    
    BORROW:
	MOVF 0x10, W
	SUBWF 0x00, W
	MOVWF 0x20
	DECF 0x20
    
    GOTO END_BORROW
    NO_BORROW:
	MOVF 0x10, W
	SUBWF 0x00, W
	MOVWF 0x20
	
    END_BORROW:
    
    
    
    
    
    
    end








