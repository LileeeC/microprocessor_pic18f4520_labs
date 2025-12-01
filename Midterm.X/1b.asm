List p=18f4520 
    #include<p18f4520.inc>
    CONFIG OSC = INTIO67
    CONFIG WDT = OFF
    org 0x00
    
    x1 EQU 0x00
    x2 EQU 0x01
    x3 EQU 0x02
    x4 EQU 0x03
    t1 EQU 0x04
    t2 EQU 0x05
    result EQU 0x10
    
    MOVLW 0x0A
    MOVWF x1
    MOVLW 0xF2
    MOVWF x2
    MOVLW 0x42
    MOVWF x3
    MOVLW 0xFA
    MOVWF x4
    
    MOVLW b'00001111'
    ANDWF x1, t1
    ANDWF x4, t2
    MOVF t2, W
    CPFSEQ tl ; skip if t1=W
    BRA No
    
    MOVLW h'10'
    ANDWF x2, t1
    ANDWF x4, t2
    MOVF t2, W
    CPFSEQ tl ; skip if t1=W
    BRA No
    
    MOVLW h'01'
    ANDWF x2, t1
    ANDWF x3, t2
    MOVF t2, W
    CPFSEQ tl ; skip if t1=W
    BRA No
    BRA Done

    No:
	MOVLW 0x00
	MOVWF result
	BRA No
    Done:
	MOVLW 0xFF
	MOVWF result
	BRA Done
    
    
END