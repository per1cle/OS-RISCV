#pragma once
#define PROC_MAX 10
struct context {    
    long ra, sp, s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;
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
    char stack[4096];
    long cpu_time;     
};
int create_process(void *entry_point, void *stack_top);
void preempt(void);  