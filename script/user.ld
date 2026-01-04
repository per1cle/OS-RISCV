OUTPUT_ARCH(riscv)
ENTRY(_start)

SECTIONS
{
    . = 0x80400000;  /* User programs start here */
    
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