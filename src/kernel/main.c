#include <stdint.h>

void uart_putstring(const char *s);

void kmain() {
    uart_putstring("Booting...\n");
    uart_putstring("Hi Barbie!\n");
    while(1);
}