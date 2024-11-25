#include "sync_methods.h"

void init_node_sync(Node* node) {
    #ifdef MUTEX_MOD
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE);

    int err = pthread_mutex_init(&(node->sync), &attr);
	pthread_mutexattr_destroy(&attr);
    if (err) {
        printf("queue_init: pthread_mutex_init() failed: %s\n", strerror(err));
        abort();
    }
    #elif defined(SPIN_MOD)
    int err = pthread_spin_init(&(node->sync), PTHREAD_PROCESS_PRIVATE);
    if (err) {
        printf("queue_init: pthread_spin_init() failed: %s\n", strerror(err));
        abort();
    }
    #else
    int err = pthread_rwlock_init(&(node->sync), PTHREAD_PROCESS_PRIVATE);
    if (err) {
        printf("queue_init: pthread_rwlock_init() failed: %s\n", strerror(err));
        abort();
    }
    #endif
}

int wrlock_node(Node* node) {
    #ifdef MUTEX_MOD
    return pthread_mutex_lock(&(node->sync));
    #elif defined(SPIN_MOD)
    return pthread_spin_lock(&(node->sync));
    #else
    return pthread_rwlock_wrlock(&(node->sync));
    #endif
}

int rdlock_node(Node* node) {
    #ifdef MUTEX_MOD
    return pthread_mutex_lock(&(node->sync));
    #elif defined(SPIN_MOD)
    return pthread_spin_lock(&(node->sync));
    #else
    return pthread_rwlock_rdlock(&(node->sync));
    #endif
}

int unlock_node(Node* node) {
    #ifdef MUTEX_MOD
    return pthread_mutex_unlock(&(node->sync));
    #elif defined(SPIN_MOD)
    return pthread_spin_unlock(&(node->sync));
    #else
    return pthread_rwlock_unlock(&(node->sync));
    #endif
}

int destroy_node_sync(Node* node) {
    #ifdef MUTEX_MOD
    int err = pthread_mutex_destroy(&(node->sync));
    if (err) {
        printf("queue_destroy: pthread_mutex_destroy() failed: %s\n", strerror(err));
    }
    return err;
    #elif defined(SPIN_MOD)
    int err = pthread_spin_destroy(&(node->sync));
    if (err) {
        printf("queue_destroy: pthread_spin_destroy() failed: %s\n", strerror(err));
    }
    return err;
    #else
    int err = pthread_rwlock_destroy(&(node->sync));
    if (err) {
        printf("queue_destroy: pthread_rwlock_destroy() failed: %s\n", strerror(err));
    }
    return err;
    #endif
}