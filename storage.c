#include "storage.h"
#include "sync_methods.h"


Node* create_node(const char* value) {
    Node* node = malloc(sizeof(Node));
    if (node == NULL) {
        return NULL;
    }

    strcpy(node->value, value);

    node->next = NULL;
    init_node_sync(node);   
    
    return node;
}

void destroy_node(Node* node) {
    destroy_node_sync(node);
    free(node);
}

unsigned int get_random_int(unsigned int min, unsigned int max) {
    return (rand() % (max - min + 1)) + min;
}

void get_random_string(char* buffer) {
    unsigned int result_length = get_random_int(0, LENGTH_STR - 1);
    buffer[result_length] = '\0';
    for (unsigned int i = 0; i < result_length; i++) {
        buffer[i] = (char)get_random_int('0', '9');
    }
}

void fill_storage(Storage* storage) {
    char buffer[LENGTH_STR];
    get_random_string(buffer);

    Node* cur_node = create_node(buffer);

    if (cur_node == NULL) {
        destroy_storage(storage);
        return;
    }

    storage->first = cur_node;

    for (unsigned int i = 0; i + 1 < STORAGE_SIZE; i++) {
        get_random_string(buffer);
        Node* new_node = create_node(buffer);

        if (new_node == NULL) {
            destroy_storage(storage);
            return;
        }
        
        new_node->next = NULL;
        cur_node->next = new_node;
        cur_node = new_node;
    }
}

Storage* create_storage() {
    Storage* storage = malloc(sizeof(Storage));
    if (storage == NULL) {
        return NULL;
    }
    storage->first = NULL;

    fill_storage(storage);

    // Node* n = storage->first;

    // while(n->next != NULL) {
    //     printf("%s\n", n->value);
    //     n = n->next;

    // }

    return storage;
}

void destroy_storage(Storage* storage) {
    Node* cur_node = storage->first;

    while (cur_node != NULL) {
        Node* next_node = cur_node->next;
        destroy_node(cur_node);
        cur_node = next_node;
    }

    free(storage);
}