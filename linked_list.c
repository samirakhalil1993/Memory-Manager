#include <stdio.h>
#include <stdint.h>
#include "memory_manager.h"

typedef struct Node {
    uint16_t data;        // Stores the data as an unsigned 16-bit integer.
    struct Node* next;    // A pointer to the next node in the list.
} Node;

void list_init(Node** head, size_t size) {
    *head = NULL;
    mem_init(size); // Initialize the memory pool with the specified size.
}


void list_insert(Node** head, uint16_t data) {
    Node* new_node = (Node*) mem_alloc(sizeof(Node));
    if (!new_node) {
        printf("Memory allocation failed\n");
        return;
    }
    new_node->data = data;
    new_node->next = NULL;

    if (*head == NULL) {
        *head = new_node;
    } else {
        Node* current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
}

void list_insert_after(Node* prev_node, uint16_t data) {
    if (prev_node == NULL) {
        printf("Previous node cannot be NULL\n");
        return;
    }

    Node* new_node = (Node*) mem_alloc(sizeof(Node));
    if (!new_node) {
        printf("Memory allocation failed\n");
        return;
    }
    new_node->data = data;
    new_node->next = prev_node->next;
    prev_node->next = new_node;
}

void list_insert_before(Node** head, Node* next_node, uint16_t data) {
    if (next_node == NULL) {
        printf("Next node cannot be NULL\n");
        return;
    }

    Node* new_node = (Node*) mem_alloc(sizeof(Node));
    if (!new_node) {
        printf("Memory allocation failed\n");
        return;
    }
    new_node->data = data;

    // If the next_node is the head, insert new_node before it.
    if (*head == next_node) {
        new_node->next = *head;
        *head = new_node;
        return;
    }

    // Find the previous node of next_node.
    Node* current = *head;
    while (current != NULL && current->next != next_node) {
        current = current->next;
    }

    if (current == NULL) {
        printf("The specified next node is not in the list\n");
        mem_free(new_node);
        return;
    }

    new_node->next = next_node;
    current->next = new_node;
}

void list_delete(Node** head, uint16_t data) {
    if (*head == NULL) {
        printf("List is empty\n");
        return;
    }

    Node* current = *head;
    Node* previous = NULL;

    // Find the node to be deleted.
    while (current != NULL && current->data != data) {
        previous = current;
        current = current->next;
    }

    // If the node was not found.
    if (current == NULL) {
        printf("Data not found in the list\n");
        return;
    }

    // Remove the node.
    if (previous == NULL) {
        // The node to be deleted is the head.
        *head = current->next;
    } else {
        previous->next = current->next;
    }

    mem_free(current);
}

Node* list_search(Node** head, uint16_t data) {
    Node* current = *head;
    while (current != NULL) {
        if (current->data == data) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void list_display(Node** head) {
    Node* current = *head;
    printf("[");
    while (current != NULL) {
        printf("%u", current->data);
        if (current->next != NULL) {
            printf(", ");
        }
        current = current->next;
    }
    printf("]");
}

void list_display_range(Node** head, Node* start_node, Node* end_node) {
    Node* current = start_node ? start_node : *head;

    printf("[");
    while (current != NULL && (end_node == NULL || current != end_node->next)){
        printf("%d", current->data);
        if (current->next != NULL && current != end_node) {
            printf(", ");
        }
        current = current->next;
    }
    printf("]");
}

int list_count_nodes(Node** head) {
    int count = 0;
    Node* current = *head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

void list_cleanup(Node** head) {
    Node* current = *head;
    while (current != NULL) {
        Node* next_node = current->next;
        mem_free(current);
        current = next_node;
    }
    *head = NULL;
    mem_deinit(); // Deinitialize the memory pool.
}
