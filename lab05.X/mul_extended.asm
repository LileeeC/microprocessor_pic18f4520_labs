#include "xc.inc"
GLOBAL  _mul_extended
PSECT   mytext, local, class=CODE, reloc=2

nL      EQU 0x01
nH      EQU 0x02
mL      EQU 0x03
mH      EQU 0x04

aL      EQU 0x10
aH      EQU 0x11
bL      EQU 0x12
bH      EQU 0x13
r0      EQU 0x20    ; result low byte
r1      EQU 0x21
r2      EQU 0x22
r3      EQU 0x23    ; result high byte
SIGN    EQU 0x30    ; 0=positive, 1=negative
tmpL    EQU 0x31
tmpH    EQU 0x32

_mul_extended:
    BCF     WDTCON, 0

    MOVFF   nL, aL
    MOVFF   nH, aH
    MOVFF   mL, bL
    MOVFF   mH, bH
    CLRF    SIGN

    ; n < 0
    BTFSS   aH, 7 ; check bit 15 (negative if == 1)
    BRA     chk_m
    COMF    aL, F
    COMF    aH, F
    INCF    aL, F
    BTFSC   STATUS, 2
    INCF    aH, F
    INCF    SIGN, F ; SIGN ^= 1

chk_m:
    ; if m < 0
    BTFSS   bH, 7
    BRA     start_mul
    COMF    bL, F
    COMF    bH, F
    INCF    bL, F
    BTFSC   STATUS, 2
    INCF    bH, F
    INCF    SIGN, F       ; SIGN ^= 1

start_mul:
    CLRF    r0
    CLRF    r1
    CLRF    r2
    CLRF    r3

    ; al * bl
    MOVF    aL, W
    MULWF   bL
    MOVFF   PRODL, r0
    MOVFF   PRODH, r1

    ;al * bh
    MOVF    aL, W
    MULWF   bH
    MOVF    PRODL, W
    ADDWF   r1, F
    MOVF    PRODH, W
    ADDWFC  r2, F
    CLRF    WREG
    ADDWFC  r3, F

    ; ah * bl
    MOVF    aH, W
    MULWF   bL
    MOVF    PRODL, W
    ADDWF   r1, F
    MOVF    PRODH, W
    ADDWFC  r2, F
    CLRF    WREG
    ADDWFC  r3, F

    ; ah * bh
    MOVF    aH, W
    MULWF   bH
    MOVF    PRODL, W
    ADDWF   r2, F
    MOVF    PRODH, W
    ADDWFC  r3, F

    ; negative then get compliment number
    BTFSS   SIGN, 0
    BRA     store_result

    COMF    r0, F
    COMF    r1, F
    COMF    r2, F
    COMF    r3, F
    INCF    r0, F
    BTFSC   STATUS, 2
    INCF    r1, F
    BTFSC   STATUS, 2
    INCF    r2, F
    BTFSC   STATUS, 2
    INCF    r3, F

store_result:
    MOVFF   r0, 0x01
    MOVFF   r1, 0x02
    MOVFF   r2, 0x03
    MOVFF   r3, 0x04
    RETURN