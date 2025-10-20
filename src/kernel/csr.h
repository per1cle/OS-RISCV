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

#define CSR_MSTATUS  0x300 // Machine Status Register
#define CSR_MTVEC    0x305 // Machine Trap Vector Base Address
#define CSR_MEPC     0x341 // Machine Exception Program Counter
#define CSR_MCAUSE   0x342 // Machine Trap Cause

//intreruperi
#define CSR_MIE      0x304 // Machine Interrupt Enable
#define CSR_MIP      0x344 // Machine Interrupt Pending

#define MSTATUS_MIE (1 << 3) // Bit-ul Machine Interrupt Enable

#define MIE_MTIE (1 << 7) // Bit-ul Machine Timer Interrupt Enable

#define MCAUSE_TIMER_M (0x80000007) // Machine Timer Interrupt