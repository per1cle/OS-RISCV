#pragma once
typedef struct {
    const char *name;
    const unsigned char *start;
    const unsigned char *end;
} ramfs_entry_t;
int exec_program(const char *name);
void exec(const char *name);
void init_ramfs(void);