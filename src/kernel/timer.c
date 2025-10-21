#include "csr.h"
#include "kernel_lib.h"

void uart_putstring(const char *s);
extern volatile long counter_tick;

#define CLINT_BASE   0x02000000L
#define CLINT_MTIMECMP (volatile long *)(CLINT_BASE + 0x4000)
#define CLINT_MTIME    (volatile long *)(CLINT_BASE + 0xBFF8)

#define TIMER_INTERVAL 100000

void tick(){
    counter_tick++;
    
    if(counter_tick % 100 == 0){
       // uart_putstring("Tick!\n");
         kernel_putstring("Tick!\n");
    }

    long next_time = *CLINT_MTIME + TIMER_INTERVAL;
    *CLINT_MTIMECMP = next_time;
}

void init_timer(void){
    long current_time = *CLINT_MTIME;
    *CLINT_MTIMECMP = current_time + TIMER_INTERVAL;

    long mie_val = read_csr(mie);
    mie_val |= MIE_MTIE;
    write_csr(mie, mie_val);

    long mstatus_val = read_csr(mstatus);
    mstatus_val |= MSTATUS_MIE;
    write_csr(mstatus, mstatus_val);

   // uart_putstring("Timer initializat\n");
   kernel_putstring("Timer initializat\n");
}
