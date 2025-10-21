#include <stdint.h>
#include "csr.h"
#include "kernel_lib.h"
#include "proc.h"

void init_timer(void);
void init_proc(void);
void scheduler(void);

void kmain() {
   // uart_putstring("Booting...\n");
   // uart_putstring("Hi Barbie!\n");
    kernel_putstring("Booting...\n");
    kernel_putstring("Hi Barbie!\n");

    init_timer();

    init_proc();

    scheduler();

    while(1);
}