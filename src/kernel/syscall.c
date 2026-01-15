#include "syscall.h"
#include "proc.h"
#include <stddef.h>
#include <stdint.h>
#define PROC_MAX 10
extern volatile long counter_tick;
extern void yield(void);
extern void kernel_putchar(char c);
extern void kernel_putstring(const char *s);
extern void kernel_print_hex(unsigned int n);
static long do_sys_write(int fd, const char *buf, long len);
static long do_sys_exit(int status);
static long do_sys_yield(void);
static long do_sys_gettime(void);
void syscall_handler(long a0, long a1, long a2, long a7, long *ret){
    long syscall_num = a7;
    switch(syscall_num){
        case SYS_WRITE:  
            *ret = do_sys_write((int)a0, (const char *)a1, a2);
            break;
        case SYS_EXIT:
            *ret = do_sys_exit((int)a0);
            break;
        case SYS_YIELD: 
            *ret = do_sys_yield();
            break;
        case SYS_GETTIME:
            *ret = do_sys_gettime();
            break;
        default:  
            kernel_putstring("[SYSCALL] Unknown syscall: ");
            kernel_print_hex((unsigned int)syscall_num);
            kernel_putstring("\n");
            *ret = SYSCALL_ERROR;
            break;
    }
}
static long do_sys_write(int fd, const char *buf, long len){
    if(fd != 1){  
        return SYSCALL_ERROR;
    }
    for(long i = 0; i < len; i++){
        kernel_putchar(buf[i]);
    }
    return len;
}
static long do_sys_exit(int status){
    extern struct proc proc_table[];
    extern int proc_actual;
    if(proc_actual > 0 && proc_actual < PROC_MAX) {
        proc_table[proc_actual].state = UNUSED;
        proc_table[proc_actual].cpu_time = 0;
        yield();
    }
    return SYSCALL_SUCCESS;
}
static long do_sys_yield(void){
    yield();  
    return SYSCALL_SUCCESS;
}
static long do_sys_gettime(void){
    return counter_tick;
}