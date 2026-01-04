#include "loader.h"
#include "proc.h"
#include "kernel_lib.h"
#include "mm.h"

// External symbols from objcopy
extern unsigned char _binary_user1_bin_start[];
extern unsigned char _binary_user1_bin_end[];
extern unsigned char _binary_user2_bin_start[];
extern unsigned char _binary_user2_bin_end[];

// RAMFS table
ramfs_entry_t ramfs[] = {
    {"user1", _binary_user1_bin_start, _binary_user1_bin_end},
    {"user2", _binary_user2_bin_start, _binary_user2_bin_end},
    {0, 0, 0}  // Sentinel
};

void init_ramfs(void) {
    kernel_putstring("RAMFS: Initialized\n");
    for(int i = 0; ramfs[i].name != 0; i++) {
        kernel_putstring("  - ");
        kernel_putstring(ramfs[i].name);
        kernel_putstring(" (");
        long size = ramfs[i].end - ramfs[i].start;
        kernel_print_hex((unsigned int)size);
        kernel_putstring(" bytes)\n");
    }
}

// Simple string compare
int strcmp(const char *s1, const char *s2) {
    while(*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

// Load program into a new process
int exec_program(const char *name) {
    // Find program in RAMFS
    ramfs_entry_t *entry = 0;
    for(int i = 0; ramfs[i].name != 0; i++) {
        if(strcmp(ramfs[i].name, name) == 0) {
            entry = &ramfs[i];
            break;
        }
    }
    
    if(!entry) {
        kernel_putstring("exec: program not found: ");
        kernel_putstring(name);
        kernel_putstring("\n");
        return -1;
    }
    
    long size = entry->end - entry->start;
    
    kernel_putstring("exec: loading ");
    kernel_putstring(name);
    kernel_putstring("...\n");
    
    // Each user program is compiled for a specific address
    // user1: 0x80400000, user2: 0x80500000
    unsigned char *prog_mem;
    if(strcmp(name, "user1") == 0) {
        prog_mem = (unsigned char*)0x80400000;
    } else if(strcmp(name, "user2") == 0) {
        prog_mem = (unsigned char*)0x80500000;
    } else {
        kernel_putstring("exec: unknown program\n");
        return -1;
    }
    
    // Copy program to memory
    for(long i = 0; i < size; i++) {
        prog_mem[i] = entry->start[i];
    }
    
    // Create process - entry point is at specific offset (_start symbol)
    // For user1/user2, _start is at offset 0x1a8 from base
    // Stack is placed 64KB after the program start
    void *entry_point = (void*)(prog_mem + 0x1a8);
    int pid = create_process(entry_point, (void*)(prog_mem + 0x10000));
    
    if(pid < 0) {
        kernel_putstring("exec: failed to create process\n");
        return -1;
    }
    
    kernel_putstring("exec: started PID ");
    kernel_putchar('0' + pid);
    kernel_putstring("\n");
    
    return pid;
}