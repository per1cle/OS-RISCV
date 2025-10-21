#pragma once

struct context {    
    long ra, sp, s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;
};

enum states {
    READY,
    UNUSED
};

struct proc {
    enum states state;
    struct context context;
    char stack[4096];
};