#include <xc.h>

#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF

// ??????? __delay_ms()
#define _XTAL_FREQ 8000000 // ???? 8MHz ???

unsigned char pattern_state = 0; 

void main(void) {

    ADCON1 = 0x0F;
    LATA = 0x00; 
    PORTA = 0x00;  // ???????
    LATB = 0x00; 
    PORTB = 0x00; 
    TRISA = 0b00000000; 
    TRISB = 0b00000001; 
    
    
    while(1) {
        
        if (PORTBbits.RB0 == 0) {

            if(pattern_state == 0){
                pattern_state = 1;
            }
            else{
                pattern_state = pattern_state << 1;
            }

            if (pattern_state > 4) {
                pattern_state = 0;
            }

            LATA = pattern_state;

            // ?????????????
            while(PORTBbits.RB0 == 0); 
            
            __delay_ms(50);

        }
        
    }
}
