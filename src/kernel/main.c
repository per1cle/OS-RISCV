#include <stdint.h>
#include "csr.h"

void uart_putstring(const char *s);
void init_timer(void);

void kmain() {
    uart_putstring("Booting...\n");
    uart_putstring("Hi Barbie!\n");

    init_timer();

    while(1);
}