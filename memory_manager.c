#include "memory_manager.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct FreeBlock {
    size_t size;
    struct FreeBlock *next;
} FreeBlock;

static char *memory_pool = NULL;
static char *current_position = NULL;
static size_t total_size = 0;
static FreeBlock *free_list = NULL;

void mem_init(size_t size) {
    if (memory_pool) {
        return;
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

    if (size == 0) {
        return current_position;
    }

    FreeBlock **current = &free_list;
    while (*current) {
        if ((*current)->size >= size) {
            void *allocated_memory = (void *)(*current);
            *current = (*current)->next;
            return allocated_memory;
        }
        current = &(*current)->next;
    }

    if (current_position + size > memory_pool + total_size) {
        return NULL;
    }

    void *allocated_memory = current_position;
    current_position += size;
    return allocated_memory;
}

void mem_free(void *ptr) {
    if (!ptr || !memory_pool || ptr < (void *)memory_pool || ptr >= (void *)(memory_pool + total_size)) {
        return;
    }

    FreeBlock *freed_block = (FreeBlock *)ptr;
    freed_block->next = free_list;
    free_list = freed_block;
}

void *mem_resize(void *ptr, size_t new_size) {
    if (!ptr) {
        return mem_alloc(new_size);
    }

    void *new_ptr = mem_alloc(new_size);
    if (new_ptr) {
        memcpy(new_ptr, ptr, new_size);
        mem_free(ptr);
    }

    return new_ptr;
}

void mem_deinit() {
    free(memory_pool);
    memory_pool = NULL;
    current_position = NULL;
    total_size = 0;
    free_list = NULL;
}
