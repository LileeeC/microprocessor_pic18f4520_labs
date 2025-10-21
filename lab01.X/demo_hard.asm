List p=18f4520
    #include<p18f4520.inc>
	CONFIG OSC = INTIO67
	CONFIG WDT = OFF
	org 0x000

	IN_VAL  EQU 0x000 ;input 8 bit
	CLZ_OUT EQU 0x010
 
	MOVLW 0xFF
	MOVWF IN_VAL

        CBLOCK 0x70
	x  ;input
	r  ;clz
	c4 ;high 4 bits all zero ? =1, else 0
	c2 ;high 2 bits all zero ? =1, else 0
	c1 ;highest bit zero ? =1, else 0
        ENDC
	
    main:
        MOVF IN_VAL,W ;move value in 0x000 to WREG
        MOVWF x      ;copy value in WREG to x
        CLRF r         ;counter=0

	;c4 = (x < 0x10) ? 1:0, if 1 then r+=4 and x<<=4
        CLRF c4 ;c4=0
        MOVLW 0x0F ;W=15
        CPFSGT x ;Compare f with W, Skip if Greater Than: if x>15 then skip next line(c4 is still 0)
        INCF c4,F ;x<=15 ? c4=1

        MOVLW 0x04 ;4*c4 add to r (branchless so using multiply)
        MULWF c4 ;PRODL=4 or 0
        MOVF PRODL,W 
        ADDWF r,F ;r+=4*c4

        BTFSC c4,0 ;check bit, if 0 then skip next line (c4=1 executes)
        BCF STATUS,C ;clear carry flag
        BTFSC c4,0 ;if c4=1 then do next line
        RLCF x,F ;Rotate Left through Carry
        BTFSC c4,0
        BCF STATUS,C
        BTFSC c4,0
        RLCF x,F
        BTFSC c4,0
        BCF STATUS,C
        BTFSC c4,0
        RLCF x,F
        BTFSC c4,0
        BCF STATUS,C
        BTFSC c4,0
        RLCF x,F

	;c2 = (x < 0x40) ? 1 : 0, if 1 then r+=2 and x<<=2
        CLRF c2
        MOVLW 0x3F ;W=63
        CPFSGT x ;x>63? then skip
        INCF c2,F ;x<=63 ? c2=1

        MOVLW 0x02 ;2*c2 add to r
        MULWF c2 ;PRODL=2 or 0
        MOVF PRODL,W
        ADDWF r,F

        BTFSC c2,0 ;c2=1 executes
        BCF STATUS,C
        BTFSC c2,0
        RLCF x,F
        BTFSC c2,0
        BCF STATUS,C
        BTFSC c2,0
        RLCF x,F

	;c1 = (x < 0x80) ? 1 : 0, if 1 then r+=1 and x<<=1
        CLRF c1
        MOVLW 0x7F ;W=127
        CPFSGT x ;x>127? then skip
        INCF c1,F ;x<=127 then c1=1

        MOVLW 0x01 ;1*c1 add to r
        MULWF c1 ;PRODL=1 or 0
        MOVF PRODL,W
        ADDWF r,F

        BTFSC c1,0 ;c1=1 executes
        BCF STATUS,C
        BTFSC c1,0
        RLCF x,F

	;r += (x == 0)
        MOVF x,F ;update Z
        MOVLW 0x01
        BTFSC STATUS,2 ;bit2 of STATUS is zero flag. Do next line if Z=1
        ADDWF r,F

	;save result
        MOVF r,W
        MOVWF CLZ_OUT

        END