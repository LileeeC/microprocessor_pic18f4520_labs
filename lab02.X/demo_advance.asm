List p=18f4520
    #include<p18f4520.inc>
	CONFIG OSC = INTIO67
	CONFIG WDT = OFF
	org 0x000

    LFSR 0, 0x200
    MOVLW 0x00
    MOVWF POSTINC0
    MOVLW 0x33
    MOVWF POSTINC0
    MOVLW 0x58
    MOVWF POSTINC0
    MOVLW 0x7A
    MOVWF POSTINC0
    MOVLW 0xC4
    MOVWF POSTINC0
    MOVLW 0xF0
    MOVWF POSTINC0
    MOVLW 0xFF
    MOVWF POSTINC0
    LFSR 0, 0x200
    
    LFSR 1, 0x210
    MOVLW 0x09
    MOVWF POSTINC1
    MOVLW 0x58
    MOVWF POSTINC1
    MOVLW 0x6E
    MOVWF POSTINC1
    MOVLW 0xB8
    MOVWF POSTINC1
    MOVLW 0xDD
    MOVWF POSTINC1
    MOVLW 0xFF
    MOVWF POSTINC1
    LFSR 1, 0x210
    
    LFSR 2, 0x220
    CNT EQU 0x20
    MOVLW d'11'
    MOVWF CNT
    
MergeLoop:
    MOVF INDF0, W ;W = current A
    CPFSLT INDF1 ;skip if f < W: if B < A, then goto CopyFromB, else goto CopyFromA
    BRA CopyFromA ;B >= A
CopyFromB:
    MOVFF POSTINC1, POSTINC2 ;C = *B, B++, C++
    BRA DeCnt
CopyFromA:
    MOVFF POSTINC0, POSTINC2 ;C = *A, A++, C++
DeCnt:
    DECFSZ CNT, f ;CNT--, write bak to the same f
    BRA MergeLoop
end