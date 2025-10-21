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

    MOVF 0x11, W ;W = subtrahend low
    SUBWF 0x01, W ;W = minuend low - W
    MOVWF 0x21 ;low result

    BC No_Borrow ;branch if carry (min > sub)
Borrow:
    ;high byte needs extra -1
    MOVF 0x10, W ;W = subtrahend high
    SUBWF 0x00, W ;W = minuend high - W
    DECF WREG, W ;extra -1
    MOVWF 0x20
    
    GOTO Done
No_Borrow:
    MOVF 0x10, W
    SUBWF 0x00, W
    MOVWF 0x20
Done:
end