#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "memory_manager.h" 

typedef struct Node {
    uint16_t data;
    struct Node* next;
} Node;

void list_init(Node** head, size_t size) {
    mem_init(64000);
    *head = NULL;
    printf("List initialized with memory pool of size: %zu bytes\n", size);
}

void list_insert(Node** head, uint16_t data) {
    printf("Inserting node with data %u\n", data);
    Node* new_node = (Node*)mem_alloc(sizeof(Node));

    if (new_node == NULL) {
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

    printf("Node with data %u inserted successfully.\n", data);
}

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
        new_node->next = *head;
        *head = new_node;
    } else {
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

void list_delete(Node** head, int data) {
    if (*head == NULL) {
        printf("List is empty. Cannot delete.\n");
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
        printf("Node with data %d not found. Cannot delete.\n", data);
        return;
    }

    prev->next = temp->next;
    mem_free(temp);

    printf("Deleted node with data %d.\n", data);
}

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

int list_count_nodes(Node** head) {
    int count = 0;
    Node* temp = *head;
    while (temp != NULL) {
        count++;
        temp = temp->next;
    }
    return count;
}

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
