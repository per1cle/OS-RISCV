#include "proc.h"
#include "csr.h"
#include "kernel_lib.h"
#define PROC_MAX 10



void comutare(struct context *old, struct context *new);

struct proc proc_table[PROC_MAX];
int proc_actual = 0;

int create_process(void *entry_point, void *stack_top) {
    // Find free slot
    for(int i = 1; i < PROC_MAX; i++) {
        if(proc_table[i].state == UNUSED) {
            proc_table[i].state = READY;
            proc_table[i].pid = i;
            proc_table[i].cpu_time = 0;
            proc_table[i].context. sp = (long)stack_top;
            proc_table[i].context.ra = (long)entry_point;
            return i;
        }
    }
    return -1;  // No free slots
}

void scheduler(void){
    proc_actual = 0;
    proc_table[proc_actual].state = READY;

    static int last_proc = 0;

    while(1){
        int next_proc = -1;
        int start = (last_proc + 1) % PROC_MAX;
        for(int i=0; i<PROC_MAX; i++){
            int index = (start + i) % PROC_MAX;

            if(index == 0){
                continue;
            }

            if(proc_table[index].state == READY){
                next_proc = index;
                break;
            }
        }

        if(next_proc == -1){
            continue;
        }

        proc_actual = next_proc;
        last_proc = proc_actual;

        comutare(&proc_table[0].context, &proc_table[proc_actual].context);

        proc_actual = 0;
    }
}

void yield(void){
    proc_table[proc_actual].state = READY;
    comutare(&proc_table[proc_actual].context, &proc_table[0].context);
}

void task1(void){
    kernel_putstring("Task 1: Testing syscalls!\n");
    
    // Test sys_write
    const char *msg = "Hello from syscall!\n";
    sys_write(1, msg, 20);
    
    while(1){
        // Test sys_gettime
        long time = sys_gettime();
        
        kernel_putstring("1[t=");
        kernel_putchar('0' + ((time / 100) % 10));
        kernel_putchar('0' + ((time / 10) % 10));
        kernel_putchar('0' + (time % 10));
        kernel_putstring("] ");
        
        for(volatile int i = 0; i < 1000000; i++);
        
        // Test sys_yield
        sys_yield();
    }
}

void task2(void){
    kernel_putstring("Task 2: Testing syscalls!\n");
    
    while(1){
        long time = sys_gettime();
        
        kernel_putstring("2[t=");
        kernel_putchar('0' + ((time / 100) % 10));
        kernel_putchar('0' + ((time / 10) % 10));
        kernel_putchar('0' + (time % 10));
        kernel_putstring("] ");
        
        for(volatile int i = 0; i < 1000000; i++);
        sys_yield();
    }
}

void init_proc(void){
    for (int i = 0; i < PROC_MAX; i++) {
        proc_table[i]. state = UNUSED;
        proc_table[i].pid = i;
        proc_table[i].cpu_time = 0;
    }
    
    //proc 1
    proc_table[1].state = READY;
    proc_table[1]. context.sp = (long)&proc_table[1].stack[4095];
    proc_table[1].context.ra = (long)task1;

    //proc 2
    proc_table[2].state = READY;
    proc_table[2].context.sp = (long)&proc_table[2].stack[4095];
    proc_table[2].context.ra = (long)task2;
}