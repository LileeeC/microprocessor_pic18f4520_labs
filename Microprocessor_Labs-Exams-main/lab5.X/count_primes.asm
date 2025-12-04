#include "xc.inc"
GLOBAL _count_primes
PSECT mytext, local, class=CODE, reloc=2


_count_primes:
    BCF WDTCON, 0
    CLRF 0x05
    CLRF 0x06; ans_temp

loop2:

    RCALL is_prime

    MOVLW 0x01
    CPFSEQ 0x34
    GOTO N_NOT_PRIME

    N_IS_PRIME:
        MOVLW 0x01
        ADDWF 0x06
        MOVLW 0x00
        ADDWFC 0x05 ; ans_temp++

    N_NOT_PRIME:


    MOVF 0x02, W
    CPFSEQ 0x04
    GOTO N_NOT_EQM
    
    MOVF 0x01, W
    CPFSEQ 0x03
    GOTO N_NOT_EQM

    N_EQM: ; n == m
        GOTO break2
    N_NOT_EQM:

    MOVLW 0x01
    ADDWF 0x01
    MOVLW 0x00
    ADDWFC 0x02 ; n++

    GOTO loop2

    break2:
    MOVFF 0x06, 0x01
    MOVFF 0x05, 0x02; ans = ans_temp

    RETURN
    
is_prime: 
    MOVFF 0x001, 0x031
    MOVFF 0x002, 0x030; n
    CLRF 0x32
    CLRF 0x33
    INCF 0x33
    INCF 0x33; i = 2

    MOVLW 0x01
    MOVWF 0x34; is_prime = true
    
    MOVLW 0x00
    CPFSEQ 0x30
    GOTO loop1
    
    MOVLW 0x02 ; n == 2
    SUBWF 0x31, W
    BZ break1
    
    MOVLW 0x01 ; n == 1
    SUBWF 0x31, W
    BZ EQZ
    
    loop1:
 
        MOVFF 0x30, 0x40
        MOVFF 0x31, 0x41; n dividend

        MOVFF 0x32, 0x42
        MOVFF 0x33, 0x43; i divisor
	
	CLRF 0x50
	CLRF 0x51
	CLRF 0x52
	CLRF 0x53

        RCALL division
        
        MOVLW 0x00
        CPFSEQ 0x53
        GOTO NOT_EQZ

        MOVLW 0x00
        CPFSEQ 0x52
        GOTO NOT_EQZ
        
        EQZ: ; if remainder == 0
            MOVLW 0xFF
            MOVWF 0x34
            GOTO break1

        NOT_EQZ: 
            
        
        MOVLW 0x01
        ADDWF 0x33
        MOVLW 0x00
        ADDWFC 0x32 ; i++

        MOVF 0x33, W
        CPFSEQ 0x31
        GOTO NOT_EQN

        MOVF 0x32, W
        CPFSEQ 0x30
        GOTO NOT_EQN

        EQN: 
            GOTO break1

        NOT_EQN:

        GOTO loop1
 
    break1:
    RETURN
    
division:
    loop:
	MOVF 0x43, W
	SUBWF 0x41

	MOVF 0x42, W ; dividend - divisor
	SUBWFB 0x40

	BTFSS STATUS, 0 ; return if dividend < 0
	GOTO break

	MOVLW 0x01
	ADDWF 0x51
	MOVLW 0x00
	ADDWFC 0x50

    GOTO loop
 
    break:
 MOVF 0x43, W
 ADDWF 0x41

 MOVF 0x42, W
 ADDWFC 0x40

 MOVFF 0x41, 0x53
 MOVFF 0x40, 0x52
 
 RETURN


