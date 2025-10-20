#include "csr.h"

void uart_putstring(const char *s);
void tick();

volatile long counter_tick = 0;

void handler_c(){
    long mcause = read_csr(mcause);
    if(mcause == MCAUSE_TIMER_M){
        tick();
    }else{
        uart_putstring("Panica panica panica\n");

        //long mepc = read_csr(CSR_MEPC);
        uart_putstring("\nMEPC: 0x");

        while(1);
    }
}

