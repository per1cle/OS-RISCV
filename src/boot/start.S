.section .text.boot
.globl boot
.globl handler_c
.globl handler
.align 2

boot: 
    la gp, gp
    la sp, stack_top

    la t0, handler
    csrw mtvec, t0

    la a0, _bss
    la a1, _bss_end

    bss_loop:
    bge a0, a1, bss_done
    
    sw zero, 0(a0)
    
    addi a0, a0, 4
    
    j bss_loop

    bss_done:
    call kmain

nope: 
    j nope

handler: 
    addi sp, sp, -128    # Mărește la 128 (32 regs * 4 bytes)
    
    sw x1, 0(sp)
    sw x3, 8(sp)
    sw x4, 12(sp)
    sw x5, 16(sp)
    sw x6, 20(sp)
    sw x7, 24(sp)
    sw x8, 28(sp)
    sw x9, 32(sp)
    sw x10, 36(sp)      # a0
    sw x11, 40(sp)      # a1
    sw x12, 44(sp)      # a2
    sw x13, 48(sp)      # a3
    sw x14, 52(sp)      # a4
    sw x15, 56(sp)      # a5
    sw x16, 60(sp)      # a6
    sw x17, 64(sp)      # a7
    sw x18, 68(sp)
    sw x19, 72(sp)
    sw x20, 76(sp)
    sw x21, 80(sp)
    sw x22, 84(sp)
    sw x23, 88(sp)
    sw x24, 92(sp)
    sw x25, 96(sp)
    sw x26, 100(sp)
    sw x27, 104(sp)
    sw x28, 108(sp)
    sw x29, 112(sp)
    sw x30, 116(sp)
    sw x31, 120(sp)

    # Încarcă registrele a0-a7 pentru handler_c
    lw a0, 36(sp)       # a0 = x10
    lw a1, 40(sp)       # a1 = x11
    lw a2, 44(sp)       # a2 = x12
    lw a3, 48(sp)       # a3 = x13
    lw a4, 52(sp)       # a4 = x14
    lw a5, 56(sp)       # a5 = x15
    lw a6, 60(sp)       # a6 = x16
    lw a7, 64(sp)       # a7 = x17
    
    call handler_c
    sw a0, 36(sp)       # Save handler_c return value
    
    # Check if we need to do context switch (check need_resched and proc_actual)
    la t0, need_resched
    lw t1, 0(t0)
    beqz t1, restore_regs  # If need_resched == 0, skip preempt
    
    la t0, proc_actual
    lw t1, 0(t0)
    beqz t1, restore_regs  # If proc_actual == 0, skip preempt (we're in scheduler)
    
    # Call preempt_from_handler(stack_frame)
    mv a0, sp           # Pass stack frame pointer as argument
    call preempt_from_handler
    # This will not return here - it will switch to scheduler
    # If it does return, something went wrong, just restore normally
    
restore_regs:
    # Restaurează registrele (inclusiv a0 care poate conține return value)
    lw x1, 0(sp)
    lw x3, 8(sp)
    lw x4, 12(sp)
    lw x5, 16(sp)
    lw x6, 20(sp)
    lw x7, 24(sp)
    lw x8, 28(sp)
    lw x9, 32(sp)
    lw x10, 36(sp)
    lw x11, 40(sp)
    lw x12, 44(sp)
    lw x13, 48(sp)
    lw x14, 52(sp)
    lw x15, 56(sp)
    lw x16, 60(sp)
    lw x17, 64(sp)
    lw x18, 68(sp)
    lw x19, 72(sp)
    lw x20, 76(sp)
    lw x21, 80(sp)
    lw x22, 84(sp)
    lw x23, 88(sp)
    lw x24, 92(sp)
    lw x25, 96(sp)
    lw x26, 100(sp)
    lw x27, 104(sp)
    lw x28, 108(sp)
    lw x29, 112(sp)
    lw x30, 116(sp)
    lw x31, 120(sp)

    addi sp, sp, 128

    mret