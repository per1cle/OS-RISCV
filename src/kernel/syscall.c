#include "syscall.h"
#include "proc.h"
#include <stddef.h>
#include <stdint.h>

// Declarații externe
extern volatile long counter_tick;
extern void yield(void);
extern void kernel_putchar(char c);
extern void kernel_putstring(const char *s);

// Forward declarations pentru funcțiile syscall
static long do_sys_write(int fd, const char *buf, long len);
static long do_sys_yield(void);
static long do_sys_gettime(void);

// Syscall dispatcher
void syscall_handler(long a0, long a1, long a2, long a7, long *ret){
    long syscall_num = a7;
    
    switch(syscall_num){
        case SYS_WRITE:  
            *ret = do_sys_write((int)a0, (const char *)a1, a2);
            break;
            
        case SYS_YIELD: 
            *ret = do_sys_yield();
            break;
            
        case SYS_GETTIME:
            *ret = do_sys_gettime();
            break;
            
        default:  
            kernel_putstring("[SYSCALL] Unknown syscall:  ");
            kernel_putchar('0' + (syscall_num % 10));
            kernel_putstring("\n");
            *ret = SYSCALL_ERROR;
            break;
    }
}

// Implementare sys_write
static long do_sys_write(int fd, const char *buf, long len){
    if(fd != 1){  // Doar stdout (UART) pentru moment
        return SYSCALL_ERROR;
    }
    
    for(long i = 0; i < len; i++){
        kernel_putchar(buf[i]);
    }
    
    return len;
}

// Implementare sys_yield
static long do_sys_yield(void){
    yield();  // Apelează yield-ul existent
    return SYSCALL_SUCCESS;
}

// Implementare sys_gettime
static long do_sys_gettime(void){
    return counter_tick;
}