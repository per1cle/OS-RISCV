#include <stddef.h> 
typedef struct block_header {
    size_t size;            
    int is_free;            
    struct block_header* next; 
} block_header_t;
void mm_init(void* heap_start, size_t heap_size);
void* kmalloc(size_t size);
void kfree(void* ptr);
void mm_stats(size_t *total, size_t *used, size_t *free_mem, int *blocks);
