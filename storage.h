#ifndef STORAGE_H
#define STORAGE_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

typedef struct _Node {
    char value[LENGTH_STR];
    struct _Node* next;
    #ifdef MUTEX_MOD
    pthread_mutex_t sync;
    #elif defined(SPIN_MOD)
    pthread_spinlock_t sync;
    #else
    pthread_rwlock_t sync;
    #endif
} Node;

typedef struct _Storage {
    Node *first;
} Storage;

Storage* create_storage();

void destroy_storage(Storage* storage);

#endif