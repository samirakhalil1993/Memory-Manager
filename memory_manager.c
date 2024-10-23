#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// Structure to represent a memory block in the pool
typedef struct Block {
    size_t size;           // Size of the block
    int is_free;           // 1 if the block is free, 0 if it is allocated
    struct Block* next;    // Pointer to the next block
    void* ptr;             // Pointer to the memory within the pool
} Block;

void* memory_pool = NULL;  // Pointer to the start of the memory pool
Block* head_block = NULL;  // Head of the linked list of memory blocks
size_t memory_pool_size = 0;

// Initializes the memory pool with the specified size
void mem_init(size_t size) {
    memory_pool = malloc(size);
    if (!memory_pool) {
        fprintf(stderr, "Memory pool allocation failed!\n");
        exit(EXIT_FAILURE);
    }

    memory_pool_size = size;

    // Create the initial block metadata
    head_block = (Block*)malloc(sizeof(Block));
    if (!head_block) {
        fprintf(stderr, "Block metadata allocation failed!\n");
        free(memory_pool);
        exit(EXIT_FAILURE);
    }

    head_block->size = size;  // The size of the entire pool
    head_block->is_free = 1;  // The entire pool is initially free
    head_block->ptr = memory_pool;  // Points to the start of the pool
    head_block->next = NULL;
}

// Allocates a block of memory of the specified size
void* mem_alloc(size_t size) {
    Block* current = head_block;

    // Find the first free block that is large enough
    while (current != NULL) {
        if (current->is_free && current->size >= size) {
            // If the block is larger than needed, split it
            if (current->size > size) {
                // Create a new metadata block for the remaining free memory
                Block* new_block = (Block*)malloc(sizeof(Block));
                if (!new_block) {
                    fprintf(stderr, "New block metadata allocation failed!\n");
                    return NULL;
                }

                new_block->size = current->size - size;
                new_block->is_free = 1;
                new_block->ptr = (char*)current->ptr + size;
                new_block->next = current->next;

                current->size = size;
                current->is_free = 0;
                current->next = new_block;
            } else {
                current->is_free = 0;
            }

            // Return the pointer to the allocated memory
            return current->ptr;
        }

        current = current->next;
    }

    // If no suitable block is found, return NULL (allocation failure)
    return NULL;
}

// Frees a previously allocated block of memory
void mem_free(void* ptr) {
    if (!ptr) return;

    // Find the block metadata corresponding to the pointer
    Block* current = head_block;
    while (current != NULL) {
        if (current->ptr == ptr) {
            current->is_free = 1;

            // Coalesce adjacent free blocks to prevent fragmentation
            Block* next_block = current->next;
            while (next_block != NULL && next_block->is_free) {
                current->size += next_block->size;
                current->next = next_block->next;
                free(next_block);
                next_block = current->next;
            }

            return;
        }

        current = current->next;
    }
}

// Resizes a previously allocated block of memory
void* mem_resize(void* ptr, size_t size) {
    if (!ptr) return mem_alloc(size); // If ptr is NULL, just allocate new memory

    Block* block = head_block;
    while (block != NULL) {
        if (block->ptr == ptr) {
            if (block->size >= size) {
                // If the current block is already large enough, return the same pointer
                return ptr;
            } else {
                // Allocate a new block and copy the old data to it
                void* new_ptr = mem_alloc(size);
                if (new_ptr) {
                    memcpy(new_ptr, ptr, block->size);
                    mem_free(ptr);
                }
                return new_ptr;
            }
        }

        block = block->next;
    }

    return NULL;  // If the block was not found
}

// Deinitializes the memory pool and frees all associated resources
void mem_deinit() {
    free(memory_pool);
    memory_pool = NULL;

    Block* current = head_block;
    while (current != NULL) {
        Block* next = current->next;
        free(current);
        current = next;
    }

    head_block = NULL;
    memory_pool_size = 0;
}
