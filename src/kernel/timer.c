#include "csr.h"
#include "kernel_lib.h"
#include "proc.h"

void uart_putstring(const char *s);
void preempt(void);  // Preemption function
extern volatile long counter_tick;
extern struct proc proc_table[];
extern int proc_actual;
volatile int need_resched = 0;  // Flag pentru preemption

#define CLINT_BASE   0x02000000L
#define CLINT_MTIMECMP (volatile long *)(CLINT_BASE + 0x4000)
#define CLINT_MTIME    (volatile long *)(CLINT_BASE + 0xBFF8)

#define TIMER_INTERVAL 100000    // Interval pentru un tick (~10ms @ 10MHz)
#define TICKS_PER_SECOND 100     // 100 ticks = 1 secundă
#define DISPLAY_INTERVAL 1000    // Afișează la fiecare 10 secunde (1000 ticks)
#define PROC_MAX 10

void tick(){
    counter_tick++;
    
    // Update CPU time for current running process
    if(proc_actual > 0 && proc_actual < PROC_MAX){
        proc_table[proc_actual].cpu_time++;
    }
    
    // Afișează timpul la fiecare DISPLAY_INTERVAL ticks (10 secunde)
    if(counter_tick % DISPLAY_INTERVAL == 0){
        long seconds = counter_tick / TICKS_PER_SECOND;
        long minutes = seconds / 60;
        long remaining_secs = seconds % 60;
        
        kernel_putstring("\n[Uptime: ");
        
        // Afișează minute dacă există
        if(minutes > 0) {
            // Print minutes
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
            // Print în ordine inversă
            while(i > 0) {
                kernel_putchar(min_str[--i]);
            }
            kernel_putstring("m ");
        }
        
        // Afișează secunde
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
        // Print în ordine inversă
        while(j > 0) {
            kernel_putchar(sec_str[--j]);
        }
        kernel_putstring("s | Ticks: ");
        
        // Afișează tick counter
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

    long next_time = *CLINT_MTIME + TIMER_INTERVAL;
    *CLINT_MTIMECMP = next_time;
    
    // PREEMPTION: Forțează context switch la fiecare tick (TRUE PREEMPTIVE)
    // Doar pentru procese user (nu idle process PID 0)
    preempt();
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

    kernel_putstring("Timer initializat\n");
}