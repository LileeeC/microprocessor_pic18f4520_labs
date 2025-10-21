; unsigned 16-bit integer division
List p=18f4520 
    #include<p18f4520.inc>
    CONFIG OSC = INTIO67
    CONFIG WDT = OFF
    org 0x00
    
    DD_high EQU 0x00
    DD_low EQU 0x01 ; Dividend 0x000/0x001
    DS_high EQU 0x02
    DS_low EQU 0x03 ; Divisor 0x002/0x003
    Q_high EQU 0x10
    Q_low EQU 0x11 ; Quotient 0x010/0x011
    R_high EQU 0x12
    R_low EQU 0x13 ; Remainder 0x012/0x013
    CNT EQU 0x020 ; count 16 times
 
    main: 
	MOVLW 0x00
	MOVWF DD_high
	MOVLW 0x64
	MOVWF DD_low
	MOVLW 0x00
	MOVWF DS_high
	MOVLW 0x0A
	MOVWF DS_low
	rcall initial
	rcall division
    finish:
	GOTO finish
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
	    ; Dividend/Remainder Shift Left (This part is correct)
	    BCF STATUS, C
	    RLCF DD_low, f
	    RLCF DD_high, f
	    RLCF R_low, f
	    RLCF R_high, f
	    
	    ; Now, shift the quotient left, carrying the 0 (from C=0) into the LSB.
	    BCF STATUS, C
	    RLCF Q_low, f
	    RLCF Q_high, f

	    ; Try R = R - Divisor (This part is correct)
	    MOVF DS_low, W
	    SUBWF R_low, f
	    MOVF DS_high, W
	    SUBWFB R_high, f

	    ; Check if subtraction was successful. 
	    ; BNC (Branch if No Carry) means C=1, which means NO BORROW occurred (R >= Divisor).
	    BNC sub_successful

	    ; --- FAILURE CASE (C=0, R < Divisor) ---
	    ; The quotient bit should be 0. We already shifted a 0 in, so we do nothing to the quotient.
	    ; BUT, we must restore the remainder by adding the divisor back.
	    MOVF  DS_low, W
	    ADDWF R_low, f
	    MOVF  DS_high, W
	    ADDWFC R_high, f
	    BRA loop_cont     ; Skip the success case and continue the loop

	sub_successful:
	    ; --- SUCCESS CASE (C=1, R >= Divisor) ---
	    ; Remainder is now correct (R - Divisor), no need to restore.
	    ; The quotient bit should be 1.
	    ; We shift the quotient left, carrying the 1 (from C=1) into the LSB.
	    BSF Q_low, 0

	loop_cont:
	    DECFSZ CNT, f
	    GOTO loop
	    RETURN
END