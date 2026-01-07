#include <stdint.h>
#define BASE 0x10000000
#define UART_THR (volatile unsigned char *)(BASE + 0x00)  // Transmit Holding Register
#define UART_RBR (volatile unsigned char *)(BASE + 0x00)  // Receive Buffer Register (same offset)
#define UART_LSR (volatile unsigned char *)(BASE + 0x05)  // Line Status Register

void kernel_putchar(char c){
    while((*UART_LSR & (1 << 5)) == 0);
    *UART_THR = c;
}

void kernel_putstring(const char *s){
    while(*s){
        kernel_putchar(*s++);
    }
}

void kernel_print_hex(unsigned int n) {   //++functia pt afisare de adrese 
    char* hex_chars = "0123456789abcdef";
    kernel_putstring("0x");

    for (int i = 28; i >= 0; i -= 4) {
        int digit = (n >> i) & 0xF;
        kernel_putchar(hex_chars[digit]); 
    }
}

// Citește un caracter din UART (blocking)
char kernel_getchar(void) {
    // Așteaptă până când data ready bit (bit 0) este setat
    while((*UART_LSR & 0x01) == 0);
    return *UART_RBR;
}

// Citește un caracter din UART (non-blocking)
// Returnează -1 dacă nu este nimic disponibil
int kernel_getchar_nonblock(void) {
    if(*UART_LSR & 0x01) {
        return *UART_RBR;
    }
    return -1;
}
