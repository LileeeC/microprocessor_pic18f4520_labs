#include "setting_hardaware/setting.h"
#include <stdlib.h>
#include "stdio.h"
#include "string.h"

#define _XTAL_FREQ 4000000

extern int ADC_Read(int channel);

int adc_val = 0;           
int mapped_val = 0;        
int old_mapped_val = -1;   
int old_adc_val_len = 0;   
const int thresholds[] = {85, 170, 256, 341, 426, 512, 597, 682, 767, 852, 938};

void Mode3(){   // Hard 
    TRISD = 0x00; 
    LATD = 0x00; 
    UART_Write_Text("ADC Mode Start\r\n");
    
    while(1){
        adc_val = ADC_Read(0);
        int buffer = 15;
        int in_buffer = 0;
        
        for(int i=0; i<11; i++) {
            if (adc_val > (thresholds[i] - buffer) && adc_val < (thresholds[i] + buffer)) {
                in_buffer = 1;
                break;
            }
        }
        
        if(in_buffer == 0){
            int temp_map = 4;
            if      (adc_val < 85)  temp_map = 4;
            else if (adc_val < 170) temp_map = 5;
            else if (adc_val < 256) temp_map = 6;
            else if (adc_val < 341) temp_map = 7;
            else if (adc_val < 426) temp_map = 8;
            else if (adc_val < 512) temp_map = 9;
            else if (adc_val < 597) temp_map = 10;
            else if (adc_val < 682) temp_map = 11;
            else if (adc_val < 767) temp_map = 12;
            else if (adc_val < 852) temp_map = 13;
            else if (adc_val < 938) temp_map = 14;
            else                    temp_map = 15;

            //  Only update triggered when output value changes
            if(temp_map != old_mapped_val) {
                __delay_ms(20);
                mapped_val = temp_map;
                LATD = mapped_val << 4; // RD4~RD7

                // UART In-Place Update
                for(int i = 0; i < old_adc_val_len; i++) {
                    UART_Write('\b');   
                    UART_Write(' ');    
                    UART_Write('\b');   
                }

                char buffer[10];
                sprintf(buffer, "%d", adc_val);
                UART_Write_Text(buffer);

                old_adc_val_len = strlen(buffer);
                old_mapped_val  = mapped_val;
            }
        }
        
        __delay_ms(10);
    }
}

void main(void) 
{
    SYSTEM_Initialize(); 
    Mode3();
    while(1);
    return;
}

void __interrupt(high_priority) Hi_ISR(void){}
void __interrupt(low_priority) Lo_ISR(void){}