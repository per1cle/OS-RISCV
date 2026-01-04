OUTPUT_ARCH(riscv)
ENTRY(_start)

SECTIONS
{
    . = 0x80500000;  /* user2 starts at different address */
    
    .text :  {
        *(.text._start)   /* Put _start first */
        *(.text*)
    }
    
    .rodata : {
        *(. rodata*)
    }
    
    .data : {
        *(.data*)
    }
    
    .bss : {
        *(.bss*)
    }
}
