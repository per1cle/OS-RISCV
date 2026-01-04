#pragma once

void kernel_putchar(char c);
void kernel_putstring(const char *s);
void kernel_print_hex(unsigned int n);

// Syscall wrapper - inline function
static inline long syscall(long num, long arg0, long arg1, long arg2) {
    register long a7 asm("a7") = num;
    register long a0 asm("a0") = arg0;
    register long a1 asm("a1") = arg1;
    register long a2 asm("a2") = arg2;
    
    asm volatile("ecall" : "+r"(a0) : "r"(a7), "r"(a1), "r"(a2) : "memory");
    return a0;
}

// Syscall helper macros - DUPĂ definirea funcției syscall
#define sys_write(fd, buf, len) syscall(64, (long)(fd), (long)(buf), (long)(len))
#define sys_yield() syscall(124, 0, 0, 0)
#define sys_gettime() syscall(169, 0, 0, 0)