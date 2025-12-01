List p=18f4520 
#include<p18f4520.inc>
CONFIG OSC = INTIO67
CONFIG WDT = OFF
org 0x00

ans EQU 0x10
xh  EQU 0x20
xl  EQU 0x21
tmp EQU 0x30
tmp2 EQU 0x31

MOVLW 0xD2
MOVWF xh
MOVLW 0xA5
MOVWF xl
CLRF tmp
 
;reverse xh
    MOVLW b'00000001' ;mask for bit 0
    ANDWF xh, W ;W = xh & mask
    RRNCF WREG ;00000001 -> 10000000
    IORWF tmp ;f = f | W
    
    MOVLW b'00000010' ;mask for bit 1
    ANDWF xh, W
    RRNCF WREG
    RRNCF WREG
    RRNCF WREG
    IORWF tmp
    
    MOVLW b'00000100' ;mask for bit 2
    ANDWF xh, W
    RLNCF WREG
    RLNCF WREG
    RLNCF WREG
    IORWF tmp
    
    MOVLW b'00001000' ;mask for bit 3
    ANDWF xh, W
    RLNCF WREG
    IORWF tmp
    
    MOVLW b'00010000' ;mask for bit 4
    ANDWF xh, W
    RRNCF WREG
    IORWF tmp
    
    MOVLW b'00100000' ;mask for bit 5
    ANDWF xh, W
    RRNCF WREG
    RRNCF WREG
    RRNCF WREG
    IORWF tmp
    
    MOVLW b'01000000' ;mask for bit 6
    ANDWF xh, W
    RLNCF WREG
    RLNCF WREG
    RLNCF WREG
    IORWF tmp
    
    MOVLW b'10000000' ;mask for bit 7
    ANDWF xh, W
    RLNCF WREG
    IORWF tmp
 
    
;compare reversed xh and original xl
MOVFF tmp, xh
RRNCF xh
MOVLW b'01111111'
ANDWF xl, f
MOVF xh, W
CPFSEQ xl
BRA No
BRA Done

No:
    MOVLW 0x00
    MOVWF ans
    BRA No
Done:
    MOVLW 0xFF
    MOVWF ans
    BRA Done
    
END