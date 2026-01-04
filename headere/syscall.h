#pragma once

// Syscall numbers (Linux-style)
#define SYS_WRITE   64
#define SYS_YIELD   124
#define SYS_GETTIME 169

// Syscall return values
#define SYSCALL_SUCCESS 0
#define SYSCALL_ERROR  -1

void syscall_handler(long a0, long a1, long a2, long a7, long *ret);