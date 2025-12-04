List p=18f4520 
    #include<p18f4520.inc>
    CONFIG OSC = INTIO67
    CONFIG WDT = OFF
    org 0x00

    
    MOVLW 0x04
    MOVWF 0x00
    
    MOVLW 0x0F
    MOVWF 0x01
    
    LFSR 0, 0x000; F(n-2)
    LFSR 1, 0x001; F(n-1)
    LFSR 2, 0x002; F(n)
    
loop:
    MOVF POSTINC0, W
    ADDWF POSTINC1, W
    MOVWF POSTINC2
    
    MOVLW 0x07
    CPFSGT FSR2L
    GOTO loop
    
    NOP
    end                   





