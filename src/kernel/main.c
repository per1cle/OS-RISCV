#include <stdint.h>
#include "csr.h"
#include "kernel_lib.h"
#include "proc.h"
#include "mm.h"
#include "shell.h"

void init_timer(void);
void init_proc(void);
void scheduler(void);

///++
extern char _heap_start[];
extern char _heap_end[];

extern void init_ramfs(void);
extern int exec_program(const char *name);

void kmain() {
   // uart_putstring("Booting...\n");
   // uart_putstring("Hi Barbie!\n");
    kernel_putstring("Booting...\n");
    kernel_putstring("Hi Barbie!\n");

    // test memorie
    kernel_putstring("MM: Initializing kernel heap...\n");
    void* heap_start_addr = (void*)_heap_start;
    size_t heap_size = (size_t)_heap_end - (size_t)_heap_start;
    
    mm_init(heap_start_addr, heap_size);
    
    kernel_putstring("--- Test 1-4 (Standard) ---\n");
    void* p1 = kmalloc(100);
    kernel_putstring("  > p1 alloc at: "); kernel_print_hex((unsigned int)p1); kernel_putstring("\n");
    
    void* p2 = kmalloc(200);
    kernel_putstring("  > p2 alloc at: "); kernel_print_hex((unsigned int)p2); kernel_putstring("\n");
    
    kfree(p1);
    kernel_putstring("  > p1 freed.\n");

    void* p3 = kmalloc(50);
    kernel_putstring("  > p3 (reusing p1) alloc at: "); kernel_print_hex((unsigned int)p3); kernel_putstring("\n");
    
    kernel_putstring("--- Test 5 (Coalescing / Lipire) ---\n");
    
    // 1. eliberam p3 (care e blocul 1) si p2 (blocul 2)
    //avem doua blocuri libere ADIACENTE: [p3 (50)] [p2 (200)]
    kernel_putstring("  > Freeing p3 (addr: "); kernel_print_hex((unsigned int)p3); kernel_putstring(")\n");
    kfree(p3);
    
    kernel_putstring("  > Freeing p2 (addr: "); kernel_print_hex((unsigned int)p2); kernel_putstring(")\n");
    kfree(p2); 
               // kfree(p2) ar trebui să vada ca 'prev' (p3) e liber si sa le lipeasca

    // (50 + 200) = 250.
    // 220 ar trebui sa încapa DOAR daca blocurile s-au lipit
    kernel_putstring("  > Attempting kmalloc(220) (should succeed if merged)...\n");
    void* p4 = kmalloc(220);
    
    if (p4) {
        kernel_putstring("  > SUCCESS! Coalescing works! Addr: ");
        //adresa ar trebui să fie cea a primului bloc (p3 sau p1)
        kernel_print_hex((unsigned int)p4); 
        kernel_putstring("\n");
    } else {
        kernel_putstring("  > FAILED! Coalescing (lipirea) NU a functionat.\n");
    }

    kernel_putstring("--- MM TEST COMPLETE ---\n");

    init_timer();

    init_ramfs();
    init_proc();

    // Inițializează shell-ul (polling mode)
    shell_init();
    
    // Lansează scheduler-ul care va apela shell_poll() în idle
    scheduler();

    while(1);
}
