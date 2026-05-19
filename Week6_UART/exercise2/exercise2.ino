#define F_CPU 16000000L
#include <avr/io.h>
#include <string.h>
#include "UART0.h"
#define TERMINATOR '\n'

int main(void) {
    int index = 0; 
    int process_data = 0;
    char buffer[100] = ""; 
    char data; 
    
    UART0_init(); 
    while (1) {
        data = UART0_read();
        if (data == TERMINATOR) { 
            buffer[index] = '\0'; 
            process_data = 1;
        } 
        else {
            buffer[index] = data;
            index++;
        }
        
        if (process_data == 1) { 
          UART0_print(buffer);
          UART0_write('\n');
            for (int i = 0 ; buffer[i] != '\0' ; i++) {
                if (buffer[i] >= 'A' && buffer[i] <= 'Z') {
                    buffer[i] += 32; 
                }
                else if (buffer[i] >= 'a' && buffer[i] <= 'z') {
                    buffer[i] -= 32;
                }
            }
            UART0_print(buffer);
            UART0_write('\n');
            index = 0;
            process_data = 0;
        }
    }
    return 0;
}

