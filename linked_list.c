#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "memory_manager.h" 

// Node structure for the singly linked list
typedef struct Node {
    uint16_t data;      // Stores the data (16-bit unsigned integer)
    struct Node* next;  // Pointer to the next node
} Node;


void list_init(Node** head, size_t size) {
    *head = NULL;  // Set head to NULL to indicate an empty list

    // Initialize the memory pool using mem_init with the given size
    printf("List initialized with memory pool of size: %zu bytes\n", size);
}





void list_insert(Node** head, uint16_t data) {
    Node* new_node = (Node*)mem_alloc(sizeof(Node));
    if (new_node == NULL) {
        printf("Memory allocation failed for new node.\n");
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

    printf("Node with data %u inserted.\n", data);
}


/**
 * Inserts a new node with the specified data immediately after the given node.
 *
 * @param prev_node A pointer to the node after which the new node will be inserted.
 * @param data The data to be inserted into the new node.
 */
void list_insert_after(Node* prev_node, int data) {
    if (prev_node == NULL) {
        printf("Previous node is NULL. Cannot insert.\n");
        return;
    }

    Node* new_node = (Node*)mem_alloc(sizeof(Node));
    if (new_node == NULL) {
        printf("Memory allocation failed. Cannot insert.\n");
        return;
    }

    new_node->data = data;
    new_node->next = prev_node->next;
    prev_node->next = new_node;

    printf("Inserted %d after node with data %d.\n", data, prev_node->data);
}

/**
 * Inserts a new node with the specified data immediately before the given node.
 *
 * @param head A pointer to the pointer of the head node of the list.
 * @param next_node A pointer to the node before which the new node will be inserted.
 * @param data The data to be inserted into the new node.
 */
void list_insert_before(Node** head, Node* next_node, int data) {
    if (next_node == NULL || *head == NULL) {
        printf("Next node or head is NULL. Cannot insert.\n");
        return;
    }

    Node* new_node = (Node*)mem_alloc(sizeof(Node));
    if (new_node == NULL) {
        printf("Memory allocation failed. Cannot insert.\n");
        return;
    }

    new_node->data = data;

    if (*head == next_node) {
        // If the next_node is the head, insert the new node at the start
        new_node->next = *head;
        *head = new_node;
    } else {
        // Find the node before next_node
        Node* temp = *head;
        while (temp != NULL && temp->next != next_node) {
            temp = temp->next;
        }

        if (temp != NULL) {
            new_node->next = temp->next;
            temp->next = new_node;
        } else {
            printf("Next node not found in the list.\n");
            mem_free(new_node);
        }
    }

    printf("Inserted %d before node with data %d.\n", data, next_node->data);
}

/**
 * Deletes a node with the specified data from the list.
 *
 * @param head A pointer to the pointer of the head node of the list.
 * @param data The data of the node to be deleted.
 */
void list_delete(Node** head, int data) {
    if (*head == NULL) {
        printf("List is empty. Cannot delete.\n");
        return;
    }

    Node* temp = *head;
    Node* prev = NULL;

    // Check if the head node contains the data
    if (temp != NULL && temp->data == data) {
        *head = temp->next;
        mem_free(temp);
        printf("Deleted node with data %d.\n", data);
        return;
    }

    // Traverse the list to find the node with the specified data
    while (temp != NULL && temp->data != data) {
        prev = temp;
        temp = temp->next;
    }

    // If the data was not found
    if (temp == NULL) {
        printf("Node with data %d not found. Cannot delete.\n", data);
        return;
    }

    // Unlink the node and free its memory
    prev->next = temp->next;
    mem_free(temp);

    printf("Deleted node with data %d.\n", data);
}


Node* list_search(Node** head, int data) {
    Node* temp = *head;  // Dereference head to get the actual list head
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
 * @param head A pointer to the pointer of the head node of the list.
 */
void list_display(Node** head) {
    Node* temp = *head;
    printf("[");
    while (temp != NULL) {
        printf("%d", temp->data);
        if (temp->next != NULL) {
            printf(", ");
        }
        temp = temp->next;
    }
    printf("]\n");
}

/**
 * Displays the nodes in the list within a specified range.
 *
 * @param head A pointer to the pointer of the head node of the list.
 * @param start A pointer to the first node to display.
 * @param end A pointer to the last node to display (NULL to display until the end of the list).
 */
void list_display_range(Node** head, Node* start, Node* end) {
    Node* temp = *head;

    // Traverse the list until we reach the start node
    while (temp != NULL && temp != start) {
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Start node not found.\n");
        return;
    }

    printf("[");
    while (temp != NULL && temp != end) {
        printf("%d", temp->data);
        if (temp->next != NULL && temp->next != end) {
            printf(", ");
        }
        temp = temp->next;
    }
    printf("]\n");
}

/**
 * Counts the number of nodes in the list.
 *
 * @param head A pointer to the pointer of the head node of the list.
 * @return The total number of nodes in the list.
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
 * @param head A pointer to the pointer of the head node of the list.
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
//
//void print_memory_pool_status(void) {
//    Block* current = free_list;
//    printf("Memory pool status:\n");
//    while (current != NULL) {
//        printf("Block at %p: size=%zu, is_free=%d\n", (void*)current, current->size, current->is_free);
//        current = current->next;
//    }
//}

/*
int main() {
    // Initialize memory pool
    mem_init(1024);

    // Initialize linked list
    Node* head = NULL;
    list_init(&head);

    // Insert elements
    list_insert(&head, 10);
    list_insert(&head, 20);
    list_insert(&head, 30);

    // Display list
    list_display(&head);

    // Search for an element
    Node* node = list_search(head, 20);
    if (node) {
        printf("Found node with data: %d\n", node->data);
    }

    // Insert before the found node
    list_insert_before(&head, node, 15);
    list_display(&head);

    // Delete a node
    list_delete(&head, 15);
    list_display(&head);

    // Count nodes
    printf("Number of nodes: %d\n", list_count_nodes(&head));

    // Cleanup list
    list_cleanup(&head);
    print_memory_pool_status();

    // Deinitialize memory pool
    mem_deinit();

    return 0;
}
*/

