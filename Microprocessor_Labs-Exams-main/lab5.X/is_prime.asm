#include "xc.inc"
GLOBAL _is_prime
PSECT mytext, local, class=CODE, reloc=2
    
_is_prime: 
    MOVWF 0x20; n
    CLRF 0x21; i
    INCF 0x21
    INCF 0x21
    
    MOVLW 0x01
    MOVWF 0x01
    
    MOVLW 0x02
    SUBWF 0x20, W
    BZ break_loop1
    
    MOVLW 0x01
    SUBWF 0x20, W
    BZ IF_BLOCK
    
    
    loop1:
 
	MOVFF 0x20, 0x30
	MOVFF 0x21, 0x31

	RCALL division

	MOVLW 0x00
	CPFSEQ 0x41
	GOTO END_IF

	IF_BLOCK: ; if remainder == 0
	    MOVLW 0xFF
	    MOVWF 0x01
	    GOTO break_loop1

	END_IF:
    
	INCF 0x21
	MOVF 0x20, W
	CPFSEQ 0x21
    GOTO loop1
 
    break_loop1:
    MOVF 0x01, W
    RETURN
    
division:
    CLRF 0x40
    
    loop_div:
    
	MOVF 0x31, W ; dividend - divisor
	SUBWF 0x30

	BTFSS STATUS, 0; return if dividend < 0 (0 = Carry Bit)
	GOTO break_loop

	INCF 0x40

    GOTO loop_div
    
    break_loop:
    MOVF 0x31, W
    ADDWF 0x30

    MOVFF 0x30, 0x41
 
 RETURN