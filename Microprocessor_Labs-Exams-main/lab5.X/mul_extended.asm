#include "xc.inc"
GLOBAL _mul_extended
PSECT mytext, local, class=CODE, reloc=2

; (AH:AL) * (BH:BL) = (AL*BL) + ((AL*BH + AH*BL)<<8) + (AH*BH<<16)

_mul_extended:
    CLRF 0x31

    BTFSS 0x02, 7
    GOTO CHECK_B
    
    MOVLW 0xFF; a is negative
    XORWF  0x01
    MOVLW 0xFF
    XORWF  0x02
    
    MOVLW 0x01
    ADDWF 0x01
    MOVLW 0x00
    ADDWFC 0x02
    
    INCF 0x31

CHECK_B:
    
    BTFSS 0x04, 7
    GOTO END_CHECK

    MOVLW 0xFF; b is negative
    XORWF  0x03
    MOVLW 0xFF
    XORWF  0x04

    MOVLW 0x01
    ADDWF 0x03
    MOVLW 0x00
    ADDWFC 0x04
    
    INCF 0x31
    
END_CHECK:
    
    MOVF 0x01, W; AL * BL
    MULWF 0x03

    MOVFF PRODL, 0x47
    MOVFF PRODH, 0x46

    MOVF 0x01, W; AL * BH
    MULWF 0x04

    MOVFF PRODL, 0x45
    MOVFF PRODH, 0x44

    MOVF 0x02, W; AH * BL
    MULWF 0x03

    MOVFF PRODL, 0x43
    MOVFF PRODH, 0x42

    MOVF 0x02, W; AH * BH
    MULWF 0x04

    MOVFF PRODL, 0x41
    MOVFF PRODH, 0x40

    MOVFF 0x47, 0x01; ALBL ALBL

    CLRF 0X30; CARRY

    MOVF 0x46, W
    ADDWF 0x45

    BTFSC  STATUS, 0
    INCF 0X30

    MOVF 0x45, W
    ADDWF 0x43

    BTFSC STATUS, 0
    INCF 0x30

    MOVFF 0x43, 0x02; 

    MOVF 0x30, W
    CLRF 0x30
    ADDWF 0x44

    BTFSC  STATUS, 0
    INCF 0X30

    MOVF 0x44, W
    ADDWF 0x42

    BTFSC STATUS, 0
    INCF 0x30

    MOVF 0x42, W
    ADDWF 0x41

    BTFSC STATUS, 0
    INCF 0x30

    MOVFF 0x41, 0x03

    MOVF 0x30, W
    ADDWF 0x40

    MOVFF 0x40, 0x04
    
    MOVLW 0x01
    CPFSEQ 0x31 ; neg_num == 1
    GOTO RES_POS
    
RES_NEG:
    MOVLW 0xFF
    XORWF  0x01
    MOVLW 0xFF
    XORWF  0x02
    MOVLW 0xFF
    XORWF  0x03
    MOVLW 0xFF
    XORWF  0x04
    
    MOVLW 0x01
    ADDWF 0x01
    MOVLW 0x00
    ADDWFC 0x02
    MOVLW 0x00
    ADDWFC 0x03
    MOVLW 0x00
    ADDWFC 0x04
    
RES_POS:

    RETURN


; register1 register2 register3 register4
; 0000 0000 0000 0000 0000 0000 0000 0000
;                     ALBL ALBL ALBL ALBL
;           ALBH ALBH ALBH ALBH
;           AHBL AHBL AHBL AHBL
; AHBH AHBH AHBH AHBH

; CARRY (0x30) neg_num (0x31) 
                    
; AHBH AHBH AHBH AHBH AHBL AHBL AHBL AHBL ALBH ALBH ALBH ALBH ALBL ALBL ALBL ALBL (0x40 ~ 0x47)

