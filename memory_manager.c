#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "memory_manager.h"
#include <assert.h>
#include <stdlib.h>
#include "common_defs.h"





// Global variables for memory management
void* memory_pool = NULL;      // Pointer to the start of the memory pool
size_t memory_pool_size = 0;   // Size of the memory pool
Block* free_list = NULL;       // Linked list of free memory blocks
//void print_memory_pool_status(void);

void mem_init(size_t size) {
    memory_pool = malloc(size);
    if (memory_pool == NULL) {
        printf("Memory pool initialization failed.\n");
        return;
    }

    memory_pool_size = size;
    free_list = (Block*)memory_pool;
    free_list->size = size - sizeof(Block);
    free_list->is_free = true;
    free_list->next = NULL;

    printf("Memory pool initialized with size: %zu bytes\n", size);
 //   print_memory_pool_status();  // Print the initial state of the memory pool
}
void* mem_alloc(size_t requested_size) {
    printf("Attempting to allocate %zu bytes\n", requested_size);
    Block* current = free_list;

    // Calculate the total size needed for this block, including metadata
    size_t total_size = requested_size + sizeof(Block);

    while (current != NULL) {
        printf("Checking block at %p: size=%zu, is_free=%d\n", (void*)current, current->size, current->is_free);

        // Check if the block is free and has enough space for requested_size + metadata
        if (current->is_free && current->size >= total_size) {
            // If the remaining space after allocation is large enough for a new block
            size_t remaining_size = current->size - total_size;

            if (remaining_size >= sizeof(Block)) {
                // Split the block: create a new free block with the remaining space
                Block* new_block = (Block*)((char*)current + total_size);
                new_block->size = remaining_size;
                new_block->is_free = true;
                new_block->next = current->next;

                // Update the current block to reflect the allocated size
                current->next = new_block;
                current->size = requested_size; // Use only the requested size for this block
            } else {
                // If there isn't enough space to create a new block, allocate the entire block
                requested_size = current->size - sizeof(Block); // Adjust the size to the full block
            }

            // Mark the current block as allocated
            current->is_free = false;
            printf("Allocated %zu bytes at %p\n", requested_size, (void*)current);
            return (char*)current + sizeof(Block); // Return the usable memory
        }

        current = current->next;
    }

    printf("Memory allocation failed: no suitable block found.\n");
    return NULL;
}





void mem_free(void* block) {
    if (block == NULL) return;

    // Get the metadata for the block
    Block* current = (Block*)((char*)block - sizeof(Block));
    current->is_free = true;

    // Coalesce with the next block if it's free
    Block* next = current->next;
    if (next != NULL && next->is_free) {
        current->size += sizeof(Block) + next->size;
        current->next = next->next; // Merge with the next block
    }

    // Coalesce with the previous block if it's free
    Block* prev = free_list;
    while (prev != NULL && prev->next != current) {
        prev = prev->next;
    }
    if (prev != NULL && prev->is_free) {
        prev->size += sizeof(Block) + current->size;
        prev->next = current->next; // Merge with the previous block
    }

    printf("Memory block freed and coalesced if possible.\n");
}



void* mem_resize(void* block, size_t size) {
    if (block == NULL) return mem_alloc(size);  // Equivalent to alloc if block is NULL

    Block* current = (Block*)((char*)block - sizeof(Block));
    
    // If the current block is large enough, return the same block
    if (current->size >= size) {
        return block;
    }

    // Try to merge with the next block if it's free and large enough
    Block* next = current->next;
    if (next != NULL && next->is_free && (current->size + sizeof(Block) + next->size >= size)) {
        current->size += sizeof(Block) + next->size;
        current->next = next->next;
        return block;  // Resizing in-place
    }

    // Otherwise, allocate a new block and move the data
    void* new_block = mem_alloc(size);
    if (new_block == NULL) {
        printf("Memory resize failed: no suitable block found.\n");
        return NULL;
    }
    
    // Copy the contents of the old block to the new block
    memcpy(new_block, block, current->size);
    mem_free(block);  // Free the old block
    
    return new_block;
}


void mem_deinit() {
    if (memory_pool != NULL) {
        // Free the entire memory pool
        free(memory_pool);
        memory_pool = NULL;
        memory_pool_size = 0;
        free_list = NULL;

        printf("Memory pool deinitialized.\n");
    }
}

//void test_exceed_cumulative_allocation() {
//    printf("Testing cumulative allocations exceeding pool size...\n");
//    
//    mem_init(1024); // Initialize with 1KB of memory
//
//    void *block1 = mem_alloc(512);
//    my_assert(block1 != NULL); // Assert block1 allocation succeeds
//
//    void *block2 = mem_alloc(512);
//    my_assert(block2 != NULL); // Assert block2 allocation succeeds
//
//    // This allocation should fail since no space is left
//    void *block3 = mem_alloc(100); 
//    my_assert(block3 == NULL); // Assert block3 allocation fails
//
//    // Freeing block1 and block2 should coalesce memory back to 1024 bytes
//    mem_free(block1);
//    mem_free(block2);
//
//    // Finally deinitialize the memory pool
//    mem_deinit();
//    printf("Cumulative allocations exceeding pool size passed.\n");
//}

//int main()
//{
//    test_exceed_cumulative_allocation();
//    return 0;
//}