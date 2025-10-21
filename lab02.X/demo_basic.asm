List p=18f4520
    #include<p18f4520.inc>
	CONFIG OSC = INTIO67
	CONFIG WDT = OFF
	org 0x000

    LFSR 0, 0x120 ;FSR0 point to [0x120]
    MOVLW 0x8E ;WREG = 0x02
    MOVWF POSTINC0, W ;[0x120] = 0x02, point to [0x121]
    MOVLW 0x37
    MOVWF POSTDEC0, W ;[0x121] = 0x03, point to [0x120]
    
    MOVF POSTINC0, W ;WREG = [0x120], point to [0x121]
    ADDWF POSTINC0, W ;WREG = [0x121]+[0x120](WREG), point to [0x122]
    MOVWF POSTDEC0 ;[0x122] = WREG, point to [0x121]
    SUBWF POSTINC0, W ;WREG = [0x121]-[0x122](WREG), point to [0x122]
    MOVWF PREINC0, W ;point to [0x123], [0x123] = WREG
    MOVWF POSTDEC0, W ;point to [0x122]
    
    MOVF POSTINC0, W 
    ADDWF POSTINC0, W 
    MOVWF POSTDEC0 
    SUBWF POSTINC0, W 
    MOVWF PREINC0, W 
    MOVWF POSTDEC0, W
    
    MOVF POSTINC0, W 
    ADDWF POSTINC0, W 
    MOVWF POSTDEC0 
    
end