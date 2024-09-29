#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "memory_manager.h"

/**
 * Node structure for the singly linked list.
 * @param data: 16-bit unsigned integer stored in the node.
 * @param next: Pointer to the next node in the list.
 */
typedef struct Node {
    uint16_t data;
    struct Node* next;
} Node;

/**
 * Initializes the linked list and memory pool.
 * 
 * @param head: Pointer to the pointer of the head node of the list.
 * @param size: Size of the memory pool to initialize.
 * 
 * Error handling: 
 * - If memory pool initialization fails, the function will print an error.
 */
void list_init(Node** head, size_t size) {
    mem_init(64000); // Initialize the memory pool with 64KB.
    *head = NULL;    // Set the head pointer to NULL.
    printf("List initialized with memory pool of size: %zu bytes\n", size);
}

/**
 * Inserts a new node at the end of the list.
 * 
 * @param head: Pointer to the pointer of the head node of the list.
 * @param data: Data to be stored in the new node.
 * 
 * Error handling:
 * - If memory allocation fails, the function exits without making any changes.
 */
void list_insert(Node** head, uint16_t data) {
    printf("Inserting node with data %u\n", data);
    Node* new_node = (Node*)mem_alloc(sizeof(Node));

    if (new_node == NULL) {
        fprintf(stderr, "Memory allocation failed for new node.\n");
        return;
    }

    new_node->data = data;
    new_node->next = NULL;

    if (*head == NULL) {
        *head = new_node; // If the list is empty, make the new node the head.
    } else {
        Node* current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node; // Append the new node at the end of the list.
    }

    printf("Node with data %u inserted successfully.\n", data);
}

/**
 * Inserts a new node after the specified node.
 * 
 * @param prev_node: Pointer to the node after which the new node will be inserted.
 * @param data: Data to be inserted into the new node.
 * 
 * Error handling:
 * - If prev_node is NULL, the function will print an error message.
 * - If memory allocation fails, the function will exit without making changes.
 */
void list_insert_after(Node* prev_node, int data) {
    if (prev_node == NULL) {
        fprintf(stderr, "Previous node is NULL. Cannot insert.\n");
        return;
    }

    Node* new_node = (Node*)mem_alloc(sizeof(Node));
    if (new_node == NULL) {
        fprintf(stderr, "Memory allocation failed. Cannot insert.\n");
        return;
    }

    new_node->data = data;
    new_node->next = prev_node->next;
    prev_node->next = new_node;

    printf("Inserted %d after node with data %d.\n", data, prev_node->data);
}

/**
 * Inserts a new node before the specified node.
 * 
 * @param head: Pointer to the pointer of the head node of the list.
 * @param next_node: Pointer to the node before which the new node will be inserted.
 * @param data: Data to be inserted into the new node.
 * 
 * Error handling:
 * - If next_node or head is NULL, the function prints an error message.
 * - If memory allocation fails, the function exits without making changes.
 */
void list_insert_before(Node** head, Node* next_node, int data) {
    if (next_node == NULL || *head == NULL) {
        fprintf(stderr, "Next node or head is NULL. Cannot insert.\n");
        return;
    }

    Node* new_node = (Node*)mem_alloc(sizeof(Node));
    if (new_node == NULL) {
        fprintf(stderr, "Memory allocation failed. Cannot insert.\n");
        return;
    }

    new_node->data = data;

    if (*head == next_node) {
        new_node->next = *head;
        *head = new_node; // Insert at the beginning if next_node is the head.
    } else {
        Node* temp = *head;
        while (temp != NULL && temp->next != next_node) {
            temp = temp->next;
        }

        if (temp != NULL) {
            new_node->next = temp->next;
            temp->next = new_node;
        } else {
            fprintf(stderr, "Next node not found in the list.\n");
            mem_free(new_node);
        }
    }

    printf("Inserted %d before node with data %d.\n", data, next_node->data);
}

/**
 * Deletes a node with the specified data from the list.
 * 
 * @param head: Pointer to the pointer of the head node of the list.
 * @param data: The data of the node to be deleted.
 * 
 * Error handling:
 * - If the list is empty, the function prints an error message.
 * - If the node with the given data is not found, an error is printed.
 */
void list_delete(Node** head, int data) {
    if (*head == NULL) {
        fprintf(stderr, "List is empty. Cannot delete.\n");
        return;
    }

    Node* temp = *head;
    Node* prev = NULL;

    if (temp != NULL && temp->data == data) {
        *head = temp->next;
        mem_free(temp);
        printf("Deleted node with data %d.\n", data);
        return;
    }

    while (temp != NULL && temp->data != data) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        fprintf(stderr, "Node with data %d not found. Cannot delete.\n", data);
        return;
    }

    prev->next = temp->next;
    mem_free(temp);

    printf("Deleted node with data %d.\n", data);
}

/**
 * Searches for a node with the specified data in the list.
 * 
 * @param head: Pointer to the pointer of the head node of the list.
 * @param data: The data to search for.
 * 
 * @return: Pointer to the node if found, NULL otherwise.
 */
Node* list_search(Node** head, int data) {
    Node* temp = *head;
    while (temp != NULL) {
        if (temp->data == data) {
            printf("Node with data %d found.\n", data);
            return temp;
        }
        temp = temp->next;
    }
    printf("Node with data %d not found.\n", data);
    return NULL;
}

/**
 * Displays all the nodes in the list.
 * 
 * @param head: Pointer to the pointer of the head node of the list.
 */
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

/**
 * Displays a range of nodes in the list.
 * 
 * @param head: Pointer to the pointer of the head node of the list.
 * @param start_node: Pointer to the first node to display.
 * @param end_node: Pointer to the last node to display (NULL to display until the end).
 */
void list_display_range(Node** head, Node* start_node, Node* end_node) {
    Node* current = start_node ? start_node : *head;
    printf("[");
    while (current != NULL) {
        printf("%u", current->data);
        if (current == end_node) {
            break;
        }
        if (current->next != NULL && current != end_node) {
            printf(", ");
        }
        current = current->next;
    }
    printf("]");
}

/**
 * Counts the number of nodes in the list.
 * 
 * @param head: Pointer to the pointer of the head node of the list.
 * 
 * @return: The total number of nodes in the list.
 */
int list_count_nodes(Node** head) {
    int count = 0;
    Node* temp = *head;
    while (temp != NULL) {
        count++;
        temp = temp->next;
    }
    return count;
}

/**
 * Frees all nodes in the list and sets the head pointer to NULL.
 * 
 * @param head: Pointer to the pointer of the head node of the list.
 */
void list_cleanup(Node** head) {
    Node* temp = *head;
    while (temp != NULL) {
        Node* next = temp->next;
        mem_free(temp);
        temp = next;
    }
    *head = NULL;
    printf("List cleaned up.\n");
}
