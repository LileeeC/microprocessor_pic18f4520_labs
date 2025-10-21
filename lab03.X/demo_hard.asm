List p=18f4520 
    #include<p18f4520.inc>
    CONFIG OSC = INTIO67
    CONFIG WDT = OFF
    org 0x00
    
    CLRF 0x00
    CLRF 0x01
    CLRF 0x02
    CLRF 0x10
    
    MOVLW 0xFE ;00000100
    MOVWF 0x00 
    MOVLW 0xFC ;00000101
    MOVWF 0x01
    MOVLW b'00000001'
    MOVWF 0x10 ;mask
loop:
    MOVF 0x10, W ;W = mask
    ANDWF 0x01, W ;check if the specific bit of [0x01] == 1
    BZ No_Add ;branch if z=1 (the bit == 0, so just move on)

    ;the bit == 1, add [0x00] to result
    MOVF 0x00, W
    ADDWF 0x02
No_Add:
    RLNCF 0x10 ;rotate left
    RLNCF 0x00 
    MOVLW b'11111110'
    ANDWF 0x00 ;[0x00] <<= 1
 
    BTFSS 0x10, 0 ;bit test f, skip if set: if mask == 00000001 then skip loop
    GOTO loop
end