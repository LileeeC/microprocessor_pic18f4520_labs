List p=18f4520 
    #include<p18f4520.inc>
    CONFIG OSC = INTIO67
    CONFIG WDT = OFF
    org 0x00
    
    MOVLW 0xB6 ; x1
    MOVWF 0x00 

    MOVLW 0x0C ; x2
    MOVWF 0x01

    MOVLW 0xD3 ; y1
    MOVWF 0x02
    
    MOVLW 0xB7 ; y2
    MOVWF 0x03

    ; x1 + x2 -> A1
    MOVF 0x001, W
    ADDWF 0x000, W
    MOVWF 0x10

    ; y1 - y2 -> A2
    MOVF 0x03, W
    SUBWF 0x02, W
    MOVWF 0x11

    MOVF 0x11, W
    CPFSGT 0x10 ; skip if A1 > A2
    GOTO ELSE_BLOCK

    IF_BLOCK:
        MOVLW 0xFF
        GOTO END_IF

    ELSE_BLOCK:
        MOVLW 0x01

    END_IF:
        MOVWF 0x20

    end                   ; ?????


