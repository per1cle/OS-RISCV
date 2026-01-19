#pragma once
#define PROC_MAX 10
struct context {    
    long ra, sp, s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;
};
struct trap_frame {
    long regs[31];  // x1-x31 (x0 is always 0)
    long mepc;      // Machine exception program counter
    long mstatus;   // Machine status register
    int valid;      // 1 if this trap frame is valid (process was preempted)
};
enum states {
    READY,
    RUNNING,  
    UNUSED
};
struct proc {
    int pid;           
    enum states state;
    struct context context;
    struct trap_frame trap_frame;
    char stack[4096];
    long cpu_time;     
};
int create_process(void *entry_point, void *stack_top);
void preempt(void);
void preempt_from_handler(void *stack_frame);  