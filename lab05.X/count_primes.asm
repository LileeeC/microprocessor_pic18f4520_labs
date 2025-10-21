#include "xc.inc"
GLOBAL  _count_primes
PSECT   mytext, local, class=CODE, reloc=2

n_low      EQU 0x001
n_high     EQU 0x002
m_low      EQU 0x003
m_high     EQU 0x004

cur_low    EQU 0x070
cur_high   EQU 0x071
mcopy_low  EQU 0x072
mcopy_high EQU 0x073
cnt_low    EQU 0x074
cnt_high   EQU 0x075
div_low    EQU 0x076
div_high   EQU 0x077
half_low   EQU 0x078
half_high  EQU 0x079
rem_low    EQU 0x07A
rem_high   EQU 0x07B
dv_low     EQU 0x07C
dv_high    EQU 0x07D
bits       EQU 0x07E

_count_primes:
    BCF     WDTCON,0

    MOVFF   n_low,  cur_low
    MOVFF   n_high, cur_high
    MOVFF   m_low,  mcopy_low
    MOVFF   m_high, mcopy_high
    CLRF    cnt_low
    CLRF    cnt_high

loop_cur:
    ; check if cur > m
    MOVF    cur_low, W
    SUBWF   mcopy_low, W
    MOVF    cur_high, W
    SUBWFB  mcopy_high, W
    BNC     prime_check         ; C=1 => cur <= m then continue
    BRA     done                ; C=0 => cur > m then stop


prime_check:
    ; skip 0 and 1
    MOVF    cur_high, W
    IORWF   cur_low, W
    BZ      inc_cur
    MOVLW   0x01
    CPFSEQ  cur_low
    BRA     start_test
    BRA     inc_cur

start_test:
    ; Compute half = cur / 2 (shift right)
    MOVFF   cur_high, half_high
    MOVFF   cur_low,  half_low
;    BCF     STATUS, C
    RRCF    half_high, F
    RRCF    half_low,  F

    ; div = 2
    CLRF    div_high
    MOVLW   0x02
    MOVWF   div_low

; Divisor loop: test divisibility for div <= half
div_loop:
    ; if (div > half) => prime
    MOVF    div_low, W
    SUBWF   half_low, W
    MOVF    div_high, W
    SUBWFB  half_high, W
    BNC     mod16_start         ; C=1 => div <= half => test it
    BRA     is_prime            ; C=0 => div > half => prime

; 16-bit division (shift?subtract method)
mod16_start:
    CLRF    rem_low
    CLRF    rem_high
    MOVFF   cur_low,  dv_low
    MOVFF   cur_high, dv_high
    MOVLW   0x10
    MOVWF   bits

mod16_loop:
    ; shift dividend and remainder left
;    BCF     STATUS, C
    RLCF    dv_low,  F
    RLCF    dv_high, F
    RLCF    rem_low, F
    RLCF    rem_high, F

    ; remainder -= div
    MOVF    div_low, W
    SUBWF   rem_low, F
    MOVF    div_high, W
    SUBWFB  rem_high, F
    BNC     mod16_borrow        ; C=1 => ok, continue
    BRA     mod16_next

mod16_borrow:
    MOVF    div_low, W
    ADDWF   rem_low, F
    MOVF    div_high, W
    ADDWFC  rem_high, F

mod16_next:
    DECF    bits, F
    BNZ     mod16_loop

    ; if remainder == 0 -> not prime
    MOVF    rem_high, W
    IORWF   rem_low, W
    BZ      not_prime

    ; else div++
    INCF    div_low, F
    BNZ     div_loop
    INCF    div_high, F
    BRA     div_loop

is_prime:
    INCF    cnt_low, F
    BNZ     inc_cur
    INCF    cnt_high, F
    BRA     inc_cur

not_prime:
inc_cur:
    ; cur++
    INCF    cur_low, F
    BNZ     loop_cur
    INCF    cur_high, F
    BRA     loop_cur

done:
    MOVFF   cnt_low,  n_low
    MOVFF   cnt_high, n_high
    MOVFF   cnt_low,  PRODL
    MOVFF   cnt_high, PRODH
    RETURN
END
