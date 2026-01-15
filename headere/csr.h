#pragma once
#define write_csr(csr_name, val) ({ \
    long __v = (long)(val); \
    asm volatile ("csrw " #csr_name ", %0" : : "r"(__v) : "memory"); \
})
#define read_csr(csr_name) ({ \
    long __v; \
    asm volatile ("csrr %0, " #csr_name : "=r"(__v) : : "memory"); \
    __v; \
})
#define CSR_MSTATUS  0x300 
#define CSR_MTVEC    0x305 
#define CSR_MEPC     0x341 
#define CSR_MCAUSE   0x342 
#define CSR_MIE      0x304 
#define CSR_MIP      0x344 
#define MSTATUS_MIE (1 << 3) 
#define MIE_MTIE (1 << 7) 
#define MCAUSE_TIMER_M (0x80000007) 