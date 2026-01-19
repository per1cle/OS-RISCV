#include "csr.h"
#include "kernel_lib.h"
void tick();
void syscall_handler(long a0, long a1, long a2, long a7, long *ret);
void preempt_from_handler(void *stack_frame);
extern int proc_actual;
volatile long counter_tick = 0;

long handler_c(long a0, long a1, long a2, long a7, long *stack_frame){
    long mcause = read_csr(mcause);
    if(mcause == MCAUSE_TIMER_M || (mcause & 0x80000000LL && (mcause & 0xFF) == 7)){
        tick();
        return a0;
    }
    else if(mcause == 8 || mcause == 11){  
        long ret = 0;
        syscall_handler(a0, a1, a2, a7, &ret);
        
        // Update MEPC on the saved stack frame so mret returns to next instruction
        // MEPC was saved at offset 124 (index 31)
        stack_frame[31] += 4;
        
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
        kernel_putstring("\nMTVAL: 0x");
        long mtval_val = read_csr(mtval);
        for(int i = 28; i >= 0; i -= 4) {
            int digit = (mtval_val >> i) & 0xF;
            kernel_putchar(digit < 10 ? '0' + digit : 'a' + digit - 10);
        }
        kernel_putstring("\n");
        while(1);
        return 0;
    }

}