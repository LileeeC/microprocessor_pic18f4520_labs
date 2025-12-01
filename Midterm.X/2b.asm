List p=18f4520
    #include<p18f4520.inc>
	CONFIG OSC = INTIO67
	CONFIG WDT = OFF
	org 0x000
	
    cnt EQU 0x50
    MOVLW 0x06
    MOVWF cnt

    LFSR 0, 0x10
    MOVLW 0x01
    MOVWF POSTINC0
    MOVLW 0x07
    MOVWF POSTINC0
    MOVLW 0x04
    MOVWF POSTINC0
    LFSR 0, 0x10

    LFSR 1, 0x20
    MOVLW 0x02
    MOVWF POSTINC1
    MOVLW 0x03
    MOVWF POSTINC1
    MOVLW 0x09
    MOVWF POSTINC1
    LFSR 1, 0x20
    
    LFSR 2, 0x40
    
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
	DECFSZ cnt, f ;CNT--, write bak to the same f
	BRA MergeLoop

	
    LFSR 0, 0x30
    MOVLW 0x08
    MOVWF POSTINC0
    MOVLW 0x07
    MOVWF POSTINC0
    MOVLW 0x05
    MOVWF POSTINC0
    LFSR 1, 0x40
    LFSR 2, 0x00
    
    MOVLW 0x08
    MOVWF cnt
    
    MergeLoop2:
	MOVF INDF0, W ;W = current A
	CPFSLT INDF1 ;skip if f < W: if B < A, then goto CopyFromB, else goto CopyFromA
	BRA CopyFromA2 ;B >= A
    CopyFromB2:
	MOVFF POSTINC1, POSTINC2 ;C = *B, B++, C++
	BRA DeCnt2
    CopyFromA2:
	MOVFF POSTINC0, POSTINC2 ;C = *A, A++, C++
    DeCnt2:
	DECFSZ cnt, f ;CNT--, write bak to the same f
	BRA MergeLoop2
    
    
    
;    
;    MergeLoop:
;	MOVF INDF0, W ;W = cur A
;	CPFSLT INDF1; skip if B < A
;	BRA BG_BvsC ; B > A
;	CPFSLT INDF2 ; skip if C < A
;	BRA AG_BvsC; C > A
;	
;	;A > B and A > C
;	MOVF INDF1, W
;	CPFSLT INDF2 ; skip if C < B
;	BRA CopyFromB ; C > B
;	BRA CopyFromC
;	
;    BG_BvsC: ;B > A, so B vs C
;	MOVF INDF1, W ; W = cur B
;	CPFSLT INDF2 ; skip if C < B
;	BRA BG_AvsC
;	BRA CopyFromA ; C > B
;    BG_AvsC:
;	MOVF INDF0, W
;	CPFSLT INDF2 ; skip if C < A
;	BRA CopyFromA ; A < C
;	BRA CopyFromC
;    AG_BvsC:
;	BRA CopyFromB
;
;    CopyFromA:
;	MOVFF POSTINC1, tmp
;	MOVFF POSTINC0, tmp ;i = *A, A++
;	BRA DeCnt
;    CopyFromB:
;	MOVFF POSTINC1, tmp ;i = *B, B++
;	BRA DeCnt
;    CopyFromC:
;	MOVFF POSTINC2, tmp ;i = *C, C++
;    DeCnt:
;	INCF i, f ;i++
;	DECFSZ cnt, f ;cnt--
;	BRA MergeLoop
END