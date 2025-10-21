List p=18f4520
    #include<p18f4520.inc>
	CONFIG OSC = INTIO67
	CONFIG WDT = OFF
	org 0x000
	
	IN_VAL  EQU 0x000
	CLZ_OUT EQU 0x010
 
	MOVLW 0xFF
	MOVWF IN_VAL

	CBLOCK  0x70 ;reserve general-purpose file registers starting at 0x70
	tmp ;copy of input value
	cnt ;counter for leading zeros
	ENDC

	main:
	    MOVF IN_VAL, W
	    MOVWF tmp ;tmp = IN_VAL
	    CLRF cnt ;cnt = 0

	    ;check if input == 0 ? CLZ = 8
	    MOVF tmp, F ;update z flag
	    BZ all_zero  ;if tmp==0 ? jump to all_zero

	clz_loop:
	    BTFSC tmp, 7 ;skip next if bit7==0
	    GOTO done ;if bit7==1, we found the first '1'

	    ;MSB=0 ? cnt+1
	    INCF cnt, F
	    RLCF tmp, F ;shift tmp left
	    BRA clz_loop ;repeat until first '1' or all bits checked

	all_zero:
	    MOVLW 8
	    MOVWF CLZ_OUT ;if input==0, clz=8
	    GOTO end_prog

	done:
	    MOVF cnt, W
	    MOVWF CLZ_OUT

	end_prog:
	    END