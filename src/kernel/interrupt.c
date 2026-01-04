#include "csr.h"
#include "kernel_lib.h"

void tick();
void syscall_handler(long a0, long a1, long a2, long a7, long *ret);

volatile long counter_tick = 0;

void handler_c(long a0, long a1, long a2, long a3, long a4, long a5, long a6, long a7){
    
    long mcause = read_csr(mcause);
    
    // Timer interrupt
    if(mcause == MCAUSE_TIMER_M){
        tick();
    }
    // Environment call (ecall) - SYSCALL
    else if(mcause == 0x0000000B){  // MCAUSE_ECALL_M (ecall from M-mode)
        long ret = 0;
        syscall_handler(a0, a1, a2, a7, &ret);
        
        // Return value în a0 - va fi restaurat la întoarcere
        // (trebuie modificat trap entry să permită asta)
        
        // Incrementăm mepc pentru a trece peste instrucțiunea ecall
        long mepc = read_csr(mepc);
        write_csr(mepc, mepc + 4);
    }
    else {
        kernel_putstring("Panica! Unknown trap:  ");
        // Afișează mcause pentru debug
        kernel_putstring("\nMCAUSE: 0x");
        while(1);
    }
}