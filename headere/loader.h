#pragma once

// RAMFS entry
typedef struct {
    const char *name;
    const unsigned char *start;
    const unsigned char *end;
} ramfs_entry_t;

// Load and execute a program
int exec_program(const char *name);

// Wrapper for shell
void exec(const char *name);

// Initialize RAMFS
void init_ramfs(void);