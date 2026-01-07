//Memory management
//lista simpla de blocuri libere

#include <stddef.h> 

//antet in fata fiecarui bloc
typedef struct block_header {
    size_t size;            //dimensiune bloc + header
    int is_free;            //1 = liber, 0 = ocupat
    struct block_header* next; //header ul urm bloc
} block_header_t;

void mm_init(void* heap_start, size_t heap_size);
void* kmalloc(size_t size);
void kfree(void* ptr);
void mm_stats(size_t *total, size_t *used, size_t *free_mem, int *blocks);
