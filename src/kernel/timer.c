#include "csr.h"
#include "kernel_lib.h"
#include "proc.h"

void uart_putstring(const char *s);
void preempt(void);  
extern volatile long counter_tick;
extern struct proc proc_table[];
extern volatile int proc_actual;
volatile int need_resched = 0;

#define CLINT_BASE   0x02000000L
#define CLINT_MTIMECMP ((volatile unsigned long *)(CLINT_BASE + 0x4000))
#define CLINT_MTIME    ((volatile unsigned long *)(CLINT_BASE + 0xBFF8))
#define TIMER_INTERVAL 100000    
#define TICKS_PER_SECOND 100     
#define DISPLAY_INTERVAL 1000    
#define PROC_MAX 10

void tick(){
    counter_tick++;
    
    // Track CPU time for current process
    if(proc_actual > 0 && proc_actual < PROC_MAX){
        proc_table[proc_actual].cpu_time++;
    }
    
    // Display uptime periodically
    if(counter_tick % DISPLAY_INTERVAL == 0){
        long seconds = counter_tick / TICKS_PER_SECOND;
        long minutes = seconds / 60;
        long remaining_secs = seconds % 60;
        kernel_putstring("\n[Uptime: ");
        if(minutes > 0) {
            char min_str[10];
            int i = 0;
            long m = minutes;
            if(m == 0) {
                min_str[i++] = '0';
            } else {
                while(m > 0) {
                    min_str[i++] = '0' + (m % 10);
                    m /= 10;
                }
            }
            while(i > 0) {
                kernel_putchar(min_str[--i]);
            }
            kernel_putstring("m ");
        }
        if(remaining_secs < 10) {
            kernel_putchar('0');
        }
        char sec_str[10];
        int j = 0;
        long s = remaining_secs;
        if(s == 0) {
            sec_str[j++] = '0';
        } else {
            while(s > 0) {
                sec_str[j++] = '0' + (s % 10);
                s /= 10;
            }
        }
        while(j > 0) {
            kernel_putchar(sec_str[--j]);
        }
        kernel_putstring("s | Ticks: ");
        char tick_str[20];
        int k = 0;
        long t = counter_tick;
        if(t == 0) {
            tick_str[k++] = '0';
        } else {
            while(t > 0) {
                tick_str[k++] = '0' + (t % 10);
                t /= 10;
            }
        }
        while(k > 0) {
            kernel_putchar(tick_str[--k]);
        }
        kernel_putstring("]\n");
    }
    
    
    unsigned long mtime_lo = CLINT_MTIME[0];
    unsigned long mtime_hi = CLINT_MTIME[1];
    if (CLINT_MTIME[1] != mtime_hi) {
        mtime_lo = CLINT_MTIME[0];
        mtime_hi = CLINT_MTIME[1];
    }
    
    unsigned long long current_time = ((unsigned long long)mtime_hi << 32) | mtime_lo;
    unsigned long long next_interrupt_val = current_time + TIMER_INTERVAL;
    
    unsigned long next_lo = (unsigned long)(next_interrupt_val & 0xFFFFFFFF);
    unsigned long next_hi = (unsigned long)(next_interrupt_val >> 32);
    
    
    CLINT_MTIMECMP[1] = 0xFFFFFFFF;
    CLINT_MTIMECMP[0] = next_lo;
    CLINT_MTIMECMP[1] = next_hi;
    
    need_resched = 1;
}

void init_timer(void){
    
    unsigned long current_time = CLINT_MTIME[0];
    unsigned long first_interrupt = current_time + TIMER_INTERVAL;
    
    
    CLINT_MTIMECMP[1] = 0xFFFFFFFF;
    CLINT_MTIMECMP[0] = first_interrupt;
    CLINT_MTIMECMP[1] = 0;
    
    long mie_val = read_csr(mie);
    mie_val |= MIE_MTIE;
    write_csr(mie, mie_val);
    long mstatus_val = read_csr(mstatus);
    mstatus_val |= MSTATUS_MIE;
    write_csr(mstatus, mstatus_val);
    kernel_putstring("Timer initializat\n");
}
