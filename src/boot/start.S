.section .text.boot
.globl boot

boot: 
    la gp, gp
    la sp, stack_top
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