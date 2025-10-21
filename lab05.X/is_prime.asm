#include "xc.inc"
GLOBAL  _is_prime
PSECT   mytext, local, class=CODE, reloc=2
    n EQU 0x001
    i EQU 0x011 
    half EQU 0x012 ; n/2
    dividend EQU 0x013 
    remainder EQU 0x014 
    cnt EQU 0x015 

_is_prime:
    MOVFF WREG, n ; input is char, so is in WREG

    ; if n < 2 return 0xFF
    MOVLW 0x02
    CPFSLT n ; skip if n < 2 (n == 1 then 0xFF)
    BRA _check
    MOVLW 0xFF
    MOVWF n
    RETURN

_check:
    ; i = 2
    MOVLW 0x02
    MOVWF i

_loop:
    ; half = n >> 1
    MOVFF n, half
    RRCF half, F ; n/2

    ; if i > half then prime
    MOVF i, W
    CPFSLT half ; skip if half < i 
    BRA _mod ; i <= half
    BRA _prime ; i > half

_mod:
    CLRF remainder
    MOVFF n, dividend ; dividend = n
    MOVLW 0x08
    MOVWF cnt ; cnt = 8

_mod_loop:
    RLCF dividend, F ; dividend <<= 1, put bit7 into C
    RLCF remainder, F ; remainder <<= 1

    ; try sub (remainder -= i)
    MOVF i, W
    SUBWF remainder, F
    BC _after_sub ; C=1 no borrow, so continue loop
    
    ; C=0 borrow, so add back i (remainder < i)
    MOVF i, W
    ADDWF remainder, F
    
_after_sub:
    DECF cnt, F ; cnt--
    BNZ _mod_loop

    ; remainder == 0 then not prime
    MOVF remainder, W
    BZ _not_prime

    INCF i, F; i++
    BRA _loop

_prime:
    MOVLW 0x01
    MOVWF n
    RETURN

_not_prime:
    MOVLW 0xFF
    MOVWF n
    RETURN
