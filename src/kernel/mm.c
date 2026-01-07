#include "mm.h"
#include "kernel_lib.h"

static block_header_t* head = NULL;  //primul bloc din heap

//adresa de start si dimensiunea totala a heap ului
void mm_init(void* heap_start, size_t heap_size) {
    if (heap_size < sizeof(block_header_t)) {
        return; 
    }

    head = (block_header_t*) heap_start;
    
    head->size = heap_size;
    head->is_free = 1;      
    head->next = NULL;    
}

void* kmalloc(size_t size) {
    if (size == 0) {
        return NULL;
    }
    
    //marime totala = marime necesara + header
    size_t total_needed = size + sizeof(block_header_t);

    //aliniere 8 bytes
    size_t size_to_alloc = (total_needed + 7) & ~7;

    block_header_t* current = head;
    while (current != NULL) {
        
        if (current->is_free == 1 && current->size >= size_to_alloc) {
            
            size_t remaining_size = current->size - size_to_alloc;
            
            if (remaining_size > (sizeof(block_header_t) + 8)) {
                
                //calculeaza adresa noului header
                block_header_t* new_free_block = (block_header_t*)((char*)current + size_to_alloc);
                
                //seteaza noul header
                new_free_block->is_free = 1;
                new_free_block->size = remaining_size;
                new_free_block->next = current->next; 

                current->is_free = 0;
                current->size = size_to_alloc;
                current->next = new_free_block;
                
            } else {
	      //daca nu merita sa l spargem il luam cu totul
                current->is_free = 0;
            }
            
            return (void*)(current + 1);
        }
        
        current = current->next;
    }

    kernel_putstring("KMALLOC: FAILED! Out of memory.\n");
    return NULL; 
}

void kfree(void* ptr)
{
    if (ptr == NULL) {
        return;
    }

    block_header_t* block_to_free = (block_header_t*)ptr - 1;

    if (block_to_free->is_free == 1) {
        kernel_putstring("MM: EROARE! Dubla eliberare (double free)!\n");
        return;
    }

    block_to_free->is_free = 1;


    //lipire
    //incearca lipirea cu blocul urmator (next)
    if (block_to_free->next != NULL && block_to_free->next->is_free == 1) {
        // kernel_putstring("MM: Lipire cu blocul 'next'...\n");
        
        //adaugam marimea blocului next la cel curent
        block_to_free->size += block_to_free->next->size;
        
        //sarim peste next si ne legam de urmatorul
        block_to_free->next = block_to_free->next->next;
    }

    //incearca lipirea cu cel anterior
    //trb sa mergem de la inceput pt ca e lista si nu putem accesa altfel
    block_header_t* iter = head;
    while (iter != NULL) {
        
        if (iter->next == block_to_free) {
            if (iter->is_free == 1) {
                // kernel_putstring("MM: Lipire cu blocul 'prev'...\n");
                
                iter->size += block_to_free->size;
                iter->next = block_to_free->next;
            }
            break; 
        }
	
        if (iter == block_to_free) {
            break;
        }

        iter = iter->next;
    }
    //sfarsit lipire
   
    #if MM_DEBUG 
    kernel_putstring("MM: kfree successful for ptr: ");
    kernel_print_hex((unsigned int)ptr);
    kernel_putstring("\n");
    #endif
}

// Funcție pentru statistici memorie
void mm_stats(size_t *total, size_t *used, size_t *free_mem, int *blocks) {
    *total = 0;
    *used = 0;
    *free_mem = 0;
    *blocks = 0;
    
    block_header_t* current = head;
    while (current != NULL) {
        (*blocks)++;
        *total += current->size;
        if (current->is_free) {
            *free_mem += current->size;
        } else {
            *used += current->size;
        }
        current = current->next;
    }
}
