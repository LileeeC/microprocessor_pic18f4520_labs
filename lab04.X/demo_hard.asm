    ; Newton?s Method using 16-bit unsigned integer arithmetic to compute the square root of a number
    ; stopping condition ?n+1 = ?n for the iteration
    List p=18f4520 
    #include<p18f4520.inc>
    CONFIG OSC = INTIO67
    CONFIG WDT = OFF
    org 0x00

    DD_high EQU 0x00
    DD_low EQU 0x01 ; Dividend
    DS_high EQU 0x02
    DS_low EQU 0x03 ; Divisor
    Q_high EQU 0x10
    Q_low EQU 0x11 ; Quotient
    R_high EQU 0x12
    R_low EQU 0x13 ; Remainder
    CNT EQU 0x34 ; Counter for 16 loops

    Xprev_high EQU 0x30
    Xprev_low EQU 0x31
    SUM_high EQU 0x32
    SUM_low EQU 0x33

    MOVLW 0x30
    MOVWF 0x20 ; N_high
    MOVLW 0x21
    MOVWF 0x21 ; N_low
    MOVLW 0x26
    MOVWF 0x22 ; x_high
    MOVLW 0x5D
    MOVWF 0x23 ; x_low
    RCALL newtonSqrt
    Stop:
	BRA Stop

    newtonSqrt:
        MOVFF 0x20, DD_high   ; Dividend = N
        MOVFF 0x21, DD_low
        MOVFF 0x22, DS_high   ; Divisor = x
        MOVFF 0x23, DS_low

	Newton_Loop:
		; backup x for checking convergence
		MOVFF DS_high, Xprev_high
		MOVFF DS_low, Xprev_low
		
		MOVFF 0x20, DD_high
		MOVFF 0x21, DD_low

		; quotient = N / x 
		RCALL initial
		RCALL division 

		; SUM = x + quotient 
		MOVF DS_low, W
		ADDWF Q_low, W
		MOVWF SUM_low
		MOVF DS_high, W
		ADDWFC Q_high, W
		MOVWF SUM_high

		; x = SUM / 2 (use right shifting)
		BCF STATUS, C
		RRCF SUM_high, f ;rotate right through carry ; bit0 -> C, bit7 <- 0
		RRCF SUM_low, f ; bit0 -> C, bit7 <- original SUM_high bit0
		MOVFF SUM_high, DS_high
		MOVFF SUM_low, DS_low

		; check converge or not
		; stopping condition: ?n+1 = ?n for the iteration
		MOVF DS_high, W
		CPFSEQ Xprev_high ; compare f and W, skip next line if f=W
		BRA Newton_Continue
		MOVF DS_low, W
		CPFSEQ Xprev_low
		BRA Newton_Continue
		BRA Newton_Done

	Newton_Continue:
		BRA Newton_Loop

	Newton_Done:
		MOVFF DS_high, 0x24
		MOVFF DS_low, 0x25
		RETURN


    initial:
	MOVLW 0x10
	MOVWF CNT
	BCF STATUS, C
	CLRF Q_high
	CLRF Q_low
	CLRF R_high
	CLRF R_low
	RETURN
	
    division:
	loop:
	    BCF STATUS, C ; C=0
	    ; Dividend << 1
	    RLCF DD_low, f
	    RLCF DD_high, f
	    ; Remainder << 1
	    RLCF R_low, f
	    RLCF R_high, f
	    
	    ; Quotient << 1
	    BCF STATUS, C
	    RLCF Q_low, f
	    RLCF Q_high, f
	    
	    ; try R = R - Divisor to see who's greater
	    MOVF DS_low, W
	    SUBWF R_low, f ; f = f-W
	    MOVF DS_high, W
	    SUBWFB R_high, f ; f = f-W-C
	    
	    BNC borrow ; Branch if No Carry (C=0)
	    ; C=0, borrow (R < divisor) -> Q=0 -> sub fail, add back divisor
	    ; C=1, no borrow (means R >= divisor) -> Q=1
	    
	    BSF Q_low, 0 ; set 0th bit = 1
	    BRA loop_cont
	    
	borrow: ; C=0
	    MOVF  DS_low, W
	    ADDWF R_low, f
	    MOVF  DS_high, W
	    ADDWFC R_high, f ; f = W+f+C
	    
	loop_cont:
	    DECFSZ CNT, f
	    GOTO loop
	    RETURN

END	