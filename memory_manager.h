#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stddef.h>  // For size_t
#include <stdbool.h> // For bool

// Define the Block structure
typedef struct Block {
    size_t size;
    bool is_free;
    struct Block* next;
} Block;

// Declare memory management functions
void mem_init(size_t size);
void* mem_alloc(size_t size);
void mem_free(void* block);
void* mem_resize(void* block, size_t size);
void mem_deinit();

#endif // MEMORY_MANAGER_H
