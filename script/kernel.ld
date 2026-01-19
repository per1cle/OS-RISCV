OUTPUT_ARCH(riscv)
ENTRY(boot)

SECTIONS {
    . = 0x80000000;
    
    .text :{
        KEEP(*(.text.boot));
        *(.text .text.*);
    }

    .rodata : ALIGN(4) {
        *(.rodata .rodata.*);
    }

    .data : ALIGN(4) {
        *(.data .data.*);
    }
    
    .bss : ALIGN(4) {
        _bss = .;
        *(.bss .bss.* .sbss .sbss.*);
        _bss_end = .;
    }

    PROVIDE(__global_pointer$ = 0);
    PROVIDE(gp = __global_pointer$);
    . = ALIGN(4);
    . = . + 0x4000;
    stack_top = .;
    
    _heap_start = .;
    _heap_end = 0x88000000; /*sfarsit RAM */
}
