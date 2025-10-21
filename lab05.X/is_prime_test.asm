#include"xc.inc"
GLOBAL _is_prime
PSECT mytext, local, class=CODE, reloc=2
; input: 1~255

_is_prime:
    MOVF 0x01, W
    MOVWF temp_n ; temp_n = n
    
    ; if n < 2, then it's not prime
    MOVLW 0x02
    CPFSLT temp_n ; skip if f < W (n<2)
    BRA Not_Prime
    
    MOVLW 0x02
    MOVWF temp_i
    
Loop:
    MOVF temp_n, W ; W = n
    MOVWF temp_half
    RRNCF temp_half, F ; temp_half = n/2 by rotating right
    MOVF temp_i, W ; W=i
    CPFSLT temp_half ; skip if f < W 
    BRA Mod   ; if i <= n/2 then go mod
    BRA Prime ; if i > n/2 then n is prime
    
Mod:
    CLRF remainder
    MOVF temp_n, W
    MOVWF dividend ; dividend = n
    MOVLW 0x08
    MOVWF CNT ; cnt = 8

Div_Loop:
    RLCF dividend, F
    RLCF remainder, F
    
    ; try remainder -= i
    MOVF temp_i, W
    SUBWF remainder, F
    BC After_Sub ; branch if C == 1 (no borrow)(remainder >= i)
    
    MOVF temp_i, W
    ADDWF remainder, F
    
After_Sub:
    DECF CNT, F ; cnt--
    BNZ Div_Loop
    
    MOVF remainder, W
    BZ Not_Prime ; remainder == 0 then not prime
    
    INCF temp_i, F ; temp_i++
    BRA Loop
    
Prime:
    ; input is prime
    MOVLW 0x01
    MOVFF WREG, 0x01
    RETURN
    
Not_Prime:
    ; input is not prime
    MOVLW 0xFF
    MOVFF WREG, 0x01
    RETURN
    
    
PSECT mydata, local, class=RAM, space=1
    ; allocate space for variables 
    temp_n: DS 1
    temp_i: DS 1
    temp_half: DS 1
    dividend: DS 1
    remainder: DS 1
    CNT: DS 1
    ; DS means asking for a "RAM space" while EQU directly specifies the address
    ; we can't use EQU now because C might conflict with ASM