List p=18f4520 
    #include<p18f4520.inc>
    CONFIG OSC = INTIO67
    CONFIG WDT = OFF
    org 0x00
    

    MOVLW 0xFF
    MOVWF 0x00 ; x

    ; int r = 0; // ?????????
    MOVLW b'00000000'
    MOVWF 0x10

    ; // ??? 4 ?
    ; c = (x < 0b00010000) << 2; // 1 << 2 = 4
    ; r += c;
    ; x <<= c;

    ; c = (x < 0b00010000) << 2; // 1 << 2 = 4
    MOVLW b'00000001'
    MOVWF 0x01 ; c = 1
    MOVLW b'00010000'
    CPFSLT 0x000
    DECF 0x01 ; if (x >= 0b00010000) c -= 1
    RLNCF 0x01
    RLNCF 0x01 ; c << 2

    ; r += c;
    MOVF 0x01, W
    ADDWF 0x10

    ; x <<= c
    MOVF 0x00, W 
    MOVWF 0x02; tempx
    RLNCF 0x02;
    RLNCF 0x02;
    RLNCF 0x02;
    RLNCF 0x02;
    MOVLW b'00000000'
    CPFSEQ 0x01
    MOVFF 0x02, 0x00

    ; // ??? 2 ?
    ; c = (x < 0b01000000) << 1; // 1 << 1 = 2
    ; r += c;
    ; x <<= c;

    ; c = (x < 0b01000000) << 1; // 1 << 1 = 2
    MOVLW b'00000001'
    MOVWF 0x01 ; c = 1
    MOVLW b'01000000'
    CPFSLT 0x00
    DECF 0x01 ; c -= 1
    RLNCF 0x01 ; c << 1

    ; r += c;
    MOVF 0x01, W
    ADDWF 0x10

    ; x <<= c
    MOVF 0x00, W 
    MOVWF 0x02; tempx
    RLNCF 0x02;
    RLNCF 0x02;
    MOVLW b'00000000'
    CPFSEQ 0x01
    MOVFF 0x02, 0x00

    ; // ???? 1 ?
    ; c = (x < 0b10000000);      // ?? 1 or 0
    ; r += c;
    ; x <<= c;

    ; c = (x < 0b10000000);      // ?? 1 or 0
    MOVLW b'00000001'
    MOVWF 0x01 ; c = 1
    MOVLW b'10000000'
    CPFSLT 0x00
    DECF 0x01 ; c -= 1

    ; r += c;
    MOVF 0x01, W
    ADDWF 0x10

    ; x <<= c
    MOVF 0x00, W 
    MOVWF 0x02; tempx
    RLNCF 0x02;
    MOVLW b'00000000'
    CPFSEQ 0x01
    MOVFF 0x02, 0x00

    ; // ?? x ? 0??????? 0?????? 32
    ; r += (x == 0);
    MOVLW b'00000001'
    MOVWF 0x02; temp 
    MOVLW b'00000000'
    CPFSEQ 0x00
    DECF 0x02
    MOVF 0x02, W 
    ADDWF 0x10
    

    end                   ; ?????