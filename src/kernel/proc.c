#include "proc.h"
#include "csr.h"
#include "kernel_lib.h"
#include "shell.h"
void comutare(struct context *old, struct context *new);
void resume_from_trap(struct trap_frame *tf);
extern volatile int need_resched;  
struct proc proc_table[PROC_MAX];
int proc_actual = 0;
int create_process(void *entry_point, void *stack_top) {
    for(int i = 1; i < PROC_MAX; i++) {
        if(proc_table[i].state == UNUSED) {
            proc_table[i].state = READY;
            proc_table[i].pid = i;
            proc_table[i].cpu_time = 0;
            proc_table[i].trap_frame.valid = 0;
            proc_table[i].context.sp = (long)stack_top;
            proc_table[i].context.ra = (long)entry_point;
            return i;
        }
    }
    return -1;  
}
void scheduler(void){
    proc_actual = 0;
    proc_table[proc_actual].state = READY;
    static int last_proc = 0;
    while(1){
        proc_actual = 0;
        
        shell_poll();
        if(need_resched) {
            need_resched = 0;
        }
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
        if(proc_table[proc_actual].trap_frame.valid) {
            proc_table[proc_actual].trap_frame.valid = 0;
            resume_from_trap(&proc_table[proc_actual].trap_frame);
        } else {
            comutare(&proc_table[0].context, &proc_table[proc_actual].context);
        }
    }
}
void yield(void){
    proc_table[proc_actual].state = READY;
    comutare(&proc_table[proc_actual].context, &proc_table[0].context);
}
void preempt(void){
    if(proc_actual > 0) {
        proc_table[proc_actual].state = READY;
        comutare(&proc_table[proc_actual].context, &proc_table[0].context);
    }
}
void preempt_from_handler(void *stack_frame){
    if(proc_actual > 0) {
        long *sf = (long *)stack_frame;
        proc_table[proc_actual].trap_frame.regs[0] = sf[0/4];
        proc_table[proc_actual].trap_frame.regs[1] = (long)stack_frame + 128;  // x2 (sp after restore)
        proc_table[proc_actual].trap_frame.regs[2] = sf[8/4];    // x3
        proc_table[proc_actual].trap_frame.regs[3] = sf[12/4];   // x4
        proc_table[proc_actual].trap_frame.regs[4] = sf[16/4];   // x5
        proc_table[proc_actual].trap_frame.regs[5] = sf[20/4];   // x6
        proc_table[proc_actual].trap_frame.regs[6] = sf[24/4];   // x7
        proc_table[proc_actual].trap_frame.regs[7] = sf[28/4];   // x8
        proc_table[proc_actual].trap_frame.regs[8] = sf[32/4];   // x9
        proc_table[proc_actual].trap_frame.regs[9] = sf[36/4];   // x10 (a0)
        proc_table[proc_actual].trap_frame.regs[10] = sf[40/4];  // x11
        proc_table[proc_actual].trap_frame.regs[11] = sf[44/4];  // x12
        proc_table[proc_actual].trap_frame.regs[12] = sf[48/4];  // x13
        proc_table[proc_actual].trap_frame.regs[13] = sf[52/4];  // x14
        proc_table[proc_actual].trap_frame.regs[14] = sf[56/4];  // x15
        proc_table[proc_actual].trap_frame.regs[15] = sf[60/4];  // x16
        proc_table[proc_actual].trap_frame.regs[16] = sf[64/4];  // x17
        proc_table[proc_actual].trap_frame.regs[17] = sf[68/4];  // x18
        proc_table[proc_actual].trap_frame.regs[18] = sf[72/4];  // x19
        proc_table[proc_actual].trap_frame.regs[19] = sf[76/4];  // x20
        proc_table[proc_actual].trap_frame.regs[20] = sf[80/4];  // x21
        proc_table[proc_actual].trap_frame.regs[21] = sf[84/4];  // x22
        proc_table[proc_actual].trap_frame.regs[22] = sf[88/4];  // x23
        proc_table[proc_actual].trap_frame.regs[23] = sf[92/4];  // x24
        proc_table[proc_actual].trap_frame.regs[24] = sf[96/4];  // x25
        proc_table[proc_actual].trap_frame.regs[25] = sf[100/4]; // x26
        proc_table[proc_actual].trap_frame.regs[26] = sf[104/4]; // x27
        proc_table[proc_actual].trap_frame.regs[27] = sf[108/4]; // x28
        proc_table[proc_actual].trap_frame.regs[28] = sf[112/4]; // x29
        proc_table[proc_actual].trap_frame.regs[29] = sf[116/4]; // x30
        proc_table[proc_actual].trap_frame.regs[30] = sf[120/4]; // x31
        proc_table[proc_actual].trap_frame.mepc = read_csr(mepc);
        proc_table[proc_actual].trap_frame.mstatus = read_csr(mstatus);
        proc_table[proc_actual].trap_frame.valid = 1;
        proc_table[proc_actual].state = READY;
        need_resched = 0;
        comutare(&proc_table[proc_actual].context, &proc_table[0].context);
    }
}
void task1(void){
    kernel_putstring("Task 1: Testing syscalls!\n");
    const char *msg = "Hello from syscall!\n";
    sys_write(1, msg, 20);
    while(1){
        long time = sys_gettime();
        kernel_putstring("1[t=");
        kernel_putchar('0' + ((time / 100) % 10));
        kernel_putchar('0' + ((time / 10) % 10));
        kernel_putchar('0' + (time % 10));
        kernel_putstring("] ");
        for(volatile int i = 0; i < 1000000; i++);
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
    proc_table[2].context.ra = (long)task2;
}