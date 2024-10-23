#include "memory_manager.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct FreeBlock {
    size_t size;               // Size of the free block.
    struct FreeBlock *next;    // Pointer to the next free block.
} FreeBlock;

static char *memory_pool = NULL;        // Pointer to the start of the memory pool.
static char *current_position = NULL;   // Pointer to the next free position in the pool.
static size_t total_size = 0;           // Total size of the memory pool.
static FreeBlock *free_list = NULL;     // Head of the free list.

// Initialize the memory pool with the given size.
void mem_init(size_t size) {
    if (memory_pool) {
        return; // Memory pool is already initialized.
    }

    memory_pool = (char *)malloc(size);
    if (memory_pool) {
        current_position = memory_pool;
        total_size = size;
        free_list = NULL;
    }
}

void *mem_alloc(size_t size) {
    if (!memory_pool) {
        return NULL;
    }

    // If size is 0, return the current position without advancing it.
    // This way, a subsequent non-zero allocation will use the same position.
    if (size == 0) {
        return current_position;
    }

    // Search the free list for a contiguous block that can satisfy the request.
    FreeBlock **current = &free_list;
    while (*current) {
        if ((*current)->size >= size) {
            // Found a suitable block; remove it from the free list.
            void *allocated_memory = (void *)(*current);
            *current = (*current)->next;  // Remove this block from the free list.
            return allocated_memory;
        }
        current = &(*current)->next;
    }

    // If no suitable free block is found, allocate from the memory pool.
    if (current_position + size > memory_pool + total_size) {
        return NULL;  // Not enough contiguous space in the pool.
    }

    // Allocate memory directly from the pool.
    void *allocated_memory = current_position;
    current_position += size;
    return allocated_memory;
}

// Free the allocated block, adding it back to the free list.
void mem_free(void *ptr) {
    if (!ptr || !memory_pool || ptr < (void *)memory_pool || ptr >= (void *)(memory_pool + total_size)) {
        return; // Invalid pointer or outside of memory pool range.
    }

    // Add the freed block back to the free list.
    FreeBlock *freed_block = (FreeBlock *)ptr;
    freed_block->next = free_list;
    free_list = freed_block;
}

// Resize a previously allocated block to a new size.
void *mem_resize(void *ptr, size_t new_size) {
    if (!ptr) {
        return mem_alloc(new_size);  // Equivalent to a new allocation if ptr is NULL.
    }

    // Allocate a new block with the requested size.
    void *new_ptr = mem_alloc(new_size);
    if (new_ptr) {
        // Copy the old data into the new block, up to the new size.
        memcpy(new_ptr, ptr, new_size);
        mem_free(ptr);
    }

    return new_ptr;
}

// Deinitialize the memory pool and reset all pointers.
void mem_deinit() {
    free(memory_pool);
    memory_pool = NULL;
    current_position = NULL;
    total_size = 0;
    free_list = NULL;
}
