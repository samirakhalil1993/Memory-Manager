#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "memory_manager.h"
#include <assert.h>
#include <stdlib.h>
#include "common_defs.h"

typedef struct Block {
    size_t size;
    bool is_free;
    struct Block* next;
} Block;

void* memory_pool = NULL;
size_t memory_pool_size = 0;
Block* free_list = NULL;

void mem_init(size_t size) {
    memory_pool = malloc(size);
    if (memory_pool == NULL) {
        return;
    }

    memory_pool_size = size;
    free_list = (Block*)memory_pool;
    free_list->size = size;
    free_list->is_free = true;
    free_list->next = NULL;
}

void* mem_alloc(size_t requested_size) {
    printf("Attempting to allocate %zu bytes\n", requested_size);
    Block* current = free_list;

    while (current != NULL) {
        printf("Checking block at %p: size=%zu, is_free=%d\n", (void*)current, current->size, current->is_free);

        if (current->is_free && current->size >= requested_size) {
            size_t remaining_size = current->size - requested_size;

            if (remaining_size > sizeof(Block)) {
                Block* new_block = (Block*)((char*)current + requested_size + sizeof(Block));
                new_block->size = remaining_size;
                new_block->is_free = true;
                new_block->next = current->next;

                current->next = new_block;
                current->size = requested_size;
            } else {
                requested_size = current->size;
            }

            current->is_free = false;
            printf("Allocated %zu bytes at %p\n", requested_size, (void*)current);

            return (char*)current + sizeof(Block);
        }

        current = current->next;
    }

    printf("Memory allocation failed: no suitable block found.\n");
    return NULL;
}

void mem_free(void* block) {
    if (block == NULL) return;

    Block* current = (Block*)((char*)block - sizeof(Block));
    current->is_free = true;

    Block* next = current->next;
    if (next != NULL && next->is_free) {
        current->size += sizeof(Block) + next->size;
        current->next = next->next;
    }

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

void* mem_resize(void* block, size_t size) {
    if (block == NULL) return mem_alloc(size);

    Block* current = (Block*)((char*)block - sizeof(Block));

    if (current->size >= size) {
        return block;
    }

    Block* next = current->next;
    if (next != NULL && next->is_free && (current->size + sizeof(Block) + next->size >= size)) {
        current->size += sizeof(Block) + next->size;
        current->next = next->next;
        return block;
    }

    void* new_block = mem_alloc(size);
    if (new_block == NULL) {
        printf("Memory resize failed: no suitable block found.\n");
        return NULL;
    }

    memcpy(new_block, block, current->size);
    mem_free(block);

    return new_block;
}

void mem_deinit() {
    if (memory_pool != NULL) {
        free(memory_pool);
        memory_pool = NULL;
        memory_pool_size = 0;
        free_list = NULL;

        printf("Memory pool deinitialized.\n");
    }
}
