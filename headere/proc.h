#pragma once

struct context {    
    long ra, sp, s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;
};

enum states {
    READY,
    RUNNING,  // Adaugă pentru tracking
    UNUSED
};

struct proc {
    int pid;           // ADAUGĂ:  Process ID
    enum states state;
    struct context context;
    char stack[4096];
    long cpu_time;     // ADAUGĂ: tick counter pentru acest proces
};