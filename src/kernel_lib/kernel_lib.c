#include <stdint.h>
#define BASE 0x10000000
#define UART_THR (volatile unsigned char *)(BASE + 0x00)  
#define UART_LSR (volatile unsigned char *)(BASE + 0x05)

void kernel_putchar(char c){
    while((*UART_LSR & (1 << 5)) == 0);
    *UART_THR = c;
}

void kernel_putstring(const char *s){
    while(*s){
        kernel_putchar(*s++);
    }
}