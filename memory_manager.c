#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "memory_manager.h"
#include <assert.h>
#include <errno.h>
#include "common_defs.h"  // Assuming this includes POSIX-compliant macros

// Structure for memory blocks in the pool
typedef struct Block {
    size_t size;        // Size of the block (usable memory)
    bool is_free;       // Block status (true if free, false if allocated)
    struct Block* next; // Pointer to the next block in the memory pool
} Block;

void* memory_pool = NULL;       // Pointer to the start of the memory pool
size_t memory_pool_size = 0;    // Total size of the memory pool
Block* free_list = NULL;        // Linked list of free blocks in the memory pool

/**
 * Initializes the memory pool.
 * 
 * @param size: The total size of the memory pool to be initialized.
 * 
 * This function allocates memory for the memory pool and sets up the first block
 * in the free list. If memory allocation fails, the function returns without
 * further action.
 */
void mem_init(size_t size) {
    memory_pool = malloc(size);  // Allocate memory for the pool
    if (memory_pool == NULL) {
        perror("Memory pool initialization failed");
        return;
    }

    memory_pool_size = size;
    free_list = (Block*)memory_pool;  // First block points to start of memory
    free_list->size = size;
    free_list->is_free = true;
    free_list->next = NULL;
}

/**
 * Allocates a memory block from the pool.
 * 
 * @param requested_size: The size of memory requested by the caller.
 * 
 * @return: A pointer to the allocated memory if successful, or NULL if the 
 * allocation fails due to insufficient space or other reasons.
 * 
 * This function searches for a free block in the memory pool that can accommodate 
 * the requested size. If a suitable block is found, it may be split into two 
 * blocks if there is enough remaining space. If no suitable block is found, 
 * the function returns NULL.
 */
void* mem_alloc(size_t requested_size) {
    printf("Attempting to allocate %zu bytes\n", requested_size);
    Block* current = free_list;

    while (current != NULL) {
        printf("Checking block at %p: size=%zu, is_free=%d\n", (void*)current, current->size, current->is_free);

        if (current->is_free && current->size >= requested_size) {
            size_t remaining_size = current->size - requested_size;

            // If there's enough space to split the block
            if (remaining_size > sizeof(Block)) {
                Block* new_block = (Block*)((char*)current + requested_size + sizeof(Block));
                new_block->size = remaining_size;
                new_block->is_free = true;
                new_block->next = current->next;

                current->next = new_block;
                current->size = requested_size;
            } else {
                requested_size = current->size;  // Allocate the whole block
            }

            current->is_free = false;
            printf("Allocated %zu bytes at %p\n", requested_size, (void*)current);

            // Return pointer to usable memory
            return (char*)current + sizeof(Block);
        }

        current = current->next;
    }

    printf("Memory allocation failed: no suitable block found.\n");
    return NULL;
}

/**
 * Frees a previously allocated memory block.
 * 
 * @param block: Pointer to the memory block to be freed.
 * 
 * The function marks the block as free and attempts to coalesce it with neighboring 
 * free blocks. If `block` is NULL, the function does nothing.
 * 
 * Error Handling: If the same block is freed more than once, the program behavior 
 * is undefined, so caution is needed to avoid double-free scenarios.
 */
void mem_free(void* block) {
    if (block == NULL) {
        return;  // Do nothing if block is NULL
    }

    Block* current = (Block*)((char*)block - sizeof(Block));
    current->is_free = true;

    // Coalesce with the next block if it's free
    Block* next = current->next;
    if (next != NULL && next->is_free) {
        current->size += sizeof(Block) + next->size;
        current->next = next->next;
    }

    // Coalesce with the previous block if it's free
    Block* prev = free_list;
    while (prev != NULL && prev->next != current) {
        prev = prev->next;
    }
    if (prev != NULL && prev->is_free) {
        prev->size += sizeof(Block) + current->size;
        prev->next = current->next;
    }

    printf("Memory block freed and coalesced if possible.\n");
}

/**
 * Resizes a previously allocated memory block.
 * 
 * @param block: Pointer to the memory block to be resized.
 * @param size: The new size for the memory block.
 * 
 * @return: A pointer to the resized memory block, or NULL if the resizing fails.
 * 
 * If the current block has enough space, it is returned without any changes. 
 * Otherwise, if the next block is free and can accommodate the new size, the 
 * current block is expanded in place. If no such block is available, a new block 
 * is allocated, and the data from the old block is copied into it.
 */
void* mem_resize(void* block, size_t size) {
    if (block == NULL) return mem_alloc(size);  // If block is NULL, allocate a new one

    Block* current = (Block*)((char*)block - sizeof(Block));

    if (current->size >= size) {
        return block;  // No need to resize if current block is already large enough
    }

    Block* next = current->next;
    if (next != NULL && next->is_free && (current->size + sizeof(Block) + next->size >= size)) {
        current->size += sizeof(Block) + next->size;
        current->next = next->next;
        return block;
    }

    // Allocate a new block if we can't expand the current one
    void* new_block = mem_alloc(size);
    if (new_block == NULL) {
        printf("Memory resize failed: no suitable block found.\n");
        return NULL;
    }

    // Copy old block's data to the new block
    memcpy(new_block, block, current->size);
    mem_free(block);  // Free the old block

    return new_block;
}

/**
 * Deinitializes the memory pool, freeing all allocated memory.
 * 
 * This function frees the entire memory pool, sets the pool pointer to NULL, and 
 * resets the free list to indicate that no memory is currently allocated.
 */
void mem_deinit() {
    if (memory_pool != NULL) {
        free(memory_pool);
        memory_pool = NULL;
        memory_pool_size = 0;
        free_list = NULL;

        printf("Memory pool deinitialized.\n");
    }
}
