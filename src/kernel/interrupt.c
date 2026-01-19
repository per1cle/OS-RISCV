#include "csr.h"
#include "kernel_lib.h"
void tick();
void syscall_handler(long a0, long a1, long a2, long a7, long *ret);
void preempt_from_handler(void *stack_frame);
extern int proc_actual;
volatile long counter_tick = 0;

long handler_c(long a0, long a1, long a2, long a3, long a4, long a5, long a6, long a7, void *stack_frame){
    long mcause = read_csr(mcause);
    if(mcause == MCAUSE_TIMER_M){
        tick();
        return a0;
    }
    else if(mcause == 0x0000000B){  
        long ret = 0;
        syscall_handler(a0, a1, a2, a7, &ret);
        long mepc = read_csr(mepc);
        write_csr(mepc, mepc + 4);
        return ret;
    }
    else {
        kernel_putstring("Panica! Unknown trap:  ");
        kernel_putstring("\nMCAUSE: 0x");
        // Print mcause in hex
        for(int i = 28; i >= 0; i -= 4) {
            int digit = (mcause >> i) & 0xF;
            kernel_putchar(digit < 10 ? '0' + digit : 'a' + digit - 10);
        }
        kernel_putstring("\nMEPC: 0x");
        long mepc_val = read_csr(mepc);
        for(int i = 28; i >= 0; i -= 4) {
            int digit = (mepc_val >> i) & 0xF;
            kernel_putchar(digit < 10 ? '0' + digit : 'a' + digit - 10);
        }
        kernel_putstring("\n");
        while(1);
        return 0;
    }

}