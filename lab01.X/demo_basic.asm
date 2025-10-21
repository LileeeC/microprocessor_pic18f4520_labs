List p=18f4520
    #include<p18f4520.inc>
	CONFIG OSC = INTIO67
	CONFIG WDT = OFF
	org 0x000

	x1 EQU 0x000
	x2 EQU 0x001
	y1 EQU 0x002
	y2 EQU 0x003
	A1 EQU 0x010
	A2 EQU 0x011
	RESULT EQU 0x020
 
	MOVLW 0xB6
	MOVWF x1
	MOVLW 0x0C
	MOVWF x2
	MOVLW 0xD3
	MOVWF y1
	MOVLW 0xB7
	MOVWF y2
 
    main:
	MOVF x1, W ;load x1 into W
	ADDWF x2, W ;W = x1 + x2
        MOVWF A1 ;store result into A1
	
	MOVF y2, W ;W = y2
        SUBWF y1, W ;W = y1 - y2
        MOVWF A2
	
	;compare A1 and A2
	MOVF A1, W
        SUBWF A2, W ;W = A1 - A2
	BNC A1_GE_A2 ;Branch if Not Carry (A1 >= A2)
	
	;case A1 < A2
	MOVLW 0x01
        MOVWF RESULT
        BRA done
	
    A1_GE_A2:
        BZ ELSE_CASE ;Branch if Zero. if zero flag set, A1 == A2
        MOVLW 0xFF ;if not zero, then A1 > A2
        MOVWF RESULT
        BRA done

    ELSE_CASE: ;A1 == A2
        MOVLW 0x01
        MOVWF RESULT

    done:
        BRA done ;infinite loop to stop program

	end