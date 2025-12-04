#include <xc.h>
#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF

// ??????? __delay_ms()
#define _XTAL_FREQ 8000000 // ???? 8MHz ???

extern void delay(void);

unsigned char state = 0;
unsigned char pattern = 0;


void main(void) {
    ADCON1 = 0x0F;
    LATA = 0x00; 
    PORTA = 0x00;  // ???????
    LATB = 0x00; 
    PORTB = 0x00; 
    TRISA = 0b00000000; 
    TRISB = 0b00000001; 
    
    while(1){
        
        if(PORTBbits.RB0 == 0){
            state++;
            pattern = 0;
            if(state > 2){
                state = 0;
            }
        }
        if(state == 0){
            LATA = 0;
            delay();
        }
        else if(state == 1){
            if(pattern == 0){
                LATA = 1;
                delay();
                pattern++;
            }
            else if(pattern == 1){
                LATA = 2;
                delay();
                pattern++;
            }
            else if(pattern == 2){
                LATA = 4;
                delay();
                pattern = 0;
            }
        }
        else if(state == 2){
            if(pattern == 0){
                LATA = 1;
                delay();
                delay();
                pattern++;
            }
            else if(pattern == 1){
                LATA = 3;
                delay();
                delay();
                pattern++;
            }
            else if(pattern == 2){
                LATA = 4;
                delay();
                if(PORTBbits.RB0 ==0){
                    continue;
                }
                LATA = 0;
                delay();
                delay();
                if(PORTBbits.RB0 ==0){
                    continue;
                }
                LATA = 4;
                delay();
                if(PORTBbits.RB0 ==0){
                    continue;
                }
                LATA = 0;
                delay();
                delay();
                if(PORTBbits.RB0 ==0){
                    continue;
                }
                LATA = 4;
                delay();
                if(PORTBbits.RB0 ==0){
                    continue;
                }
                pattern = 0;
            }
        }
    }
    return;
}
