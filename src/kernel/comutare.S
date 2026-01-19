.section .text
.globl comutare
.globl resume_from_trap
.align 2

comutare:
    sw ra, 0(a0)
    sw sp, 4(a0)
    sw s0, 8(a0)  
    sw s1, 12(a0)
    sw s2, 16(a0)
    sw s3, 20(a0)
    sw s4, 24(a0)
    sw s5, 28(a0)
    sw s6, 32(a0)
    sw s7, 36(a0)
    sw s8, 40(a0)
    sw s9, 44(a0)
    sw s10, 48(a0)
    sw s11, 52(a0)

    lw ra, 0(a1)
    lw sp, 4(a1)
    lw s0, 8(a1)
    lw s1, 12(a1)
    lw s2, 16(a1)
    lw s3, 20(a1)
    lw s4, 24(a1)
    lw s5, 28(a1)
    lw s6, 32(a1)
    lw s7, 36(a1)
    lw s8, 40(a1)
    lw s9, 44(a1)
    lw s10, 48(a1)
    lw s11, 52(a1)

    ret

.globl entry_point_wrapper
entry_point_wrapper:
    li t0, 0x8
    csrs mstatus, t0   # Set MIE (Enable Interrupts)
    jr s0              # Jump to real entry point


# resume_from_trap(struct trap_frame *tf)
# Restores all registers from trap frame and returns to user process with mret
resume_from_trap:
    mv t6, a0           # t6 = &trap_frame.regs[0]
    
    # Restore all registers except sp and t6 (we're using t6)
    lw x1, 0(t6)        # ra
    # sp will be restored last
    lw x3, 8(t6)        # gp
    lw x4, 12(t6)       # tp
    lw x5, 16(t6)       # t0
    lw x6, 20(t6)       # t1
    lw x7, 24(t6)       # t2
    lw x8, 28(t6)       # s0/fp
    lw x9, 32(t6)       # s1
    lw x10, 36(t6)      # a0
    lw x11, 40(t6)      # a1
    lw x12, 44(t6)      # a2
    lw x13, 48(t6)      # a3
    lw x14, 52(t6)      # a4
    lw x15, 56(t6)      # a5
    lw x16, 60(t6)      # a6
    lw x17, 64(t6)      # a7
    lw x18, 68(t6)      # s2
    lw x19, 72(t6)      # s3
    lw x20, 76(t6)      # s4
    lw x21, 80(t6)      # s5
    lw x22, 84(t6)      # s6
    lw x23, 88(t6)      # s7
    lw x24, 92(t6)      # s8
    lw x25, 96(t6)      # s9
    lw x26, 100(t6)     # s10
    lw x27, 104(t6)     # s11
    lw x28, 108(t6)     # t3
    lw x29, 112(t6)     # t4
    lw x30, 116(t6)     # t5
    # x31 is t6, restore it last
    lw x2, 4(t6)        # sp
    
    # Restore mepc and mstatus (offset 124 = 31*4, offset 128 = 32*4)
    lw t0, 124(t6)
    csrw mepc, t0
    
    
    lw t0, 128(t6)
    
    # Force MPIE=1 (bit 7)
    li t1, 0x80
    or t0, t0, t1
    
    # Force MPP=3 (Machine Mode) (bits 11:12)
    li t1, 0x1800
    or t0, t0, t1

    # Force MIE=0 (bit 3) - mret will copy MPIE to MIE
    li t1, 0x8
    not t1, t1
    and t0, t0, t1

    csrw mstatus, t0
    
    lw x31, 120(t6)     # t6
    
    mret