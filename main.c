#define _GNU_SOURCE

#include <stdatomic.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sched.h>

#include "storage.h"
#include "sync_methods.h"

unsigned int greater_counter_iteration = 0;
unsigned int greater_counter_string = 0;

unsigned int less_counter_iteration = 0;
unsigned int less_counter_string = 0;

unsigned int equal_counter_iteration = 0;
unsigned int equal_counter_string = 0;

unsigned int swap_counter_iteration = 0;
unsigned int swap_counter = 0;

pthread_mutex_t mutex;

void set_cpu(int n) {
    int err;
    cpu_set_t cpuset;
    pthread_t tid = pthread_self();

    CPU_ZERO(&cpuset);
    CPU_SET(n, &cpuset);

    err = pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpuset);
    if (err) {
        printf("set_cpu: pthread_setaffinity failed for cpu %d\n", n);
        return;
    }
}

void* greater_cmp(void *arg) {
    set_cpu(0);

    Storage* storage = (Storage*)arg;
	printf("greater_cmp [%d %d %d]\n", getpid(), getppid(), gettid());

    while (1) {
        Node* cur_node = storage->first;
        if (cur_node == NULL) break;
        rdlock_node(cur_node);

        Node* next_node = cur_node->next;

        int local_greater_count = 0;

        while (next_node != NULL) {
            rdlock_node(next_node);
            if (strlen(cur_node->value) > strlen(next_node->value)) {
                local_greater_count++;
            }
            unlock_node(cur_node);
            cur_node = next_node;
            next_node = cur_node->next;
        }

        greater_counter_string = local_greater_count;

        unlock_node(cur_node);
        greater_counter_iteration++;
    }

	return NULL;
}

void* less_cmp(void *arg) {
    set_cpu(1);

    Storage* storage = (Storage*)arg;
	printf("less_cmp [%d %d %d]\n", getpid(), getppid(), gettid());

    while (1) {
        Node* cur_node = storage->first;

        if (cur_node == NULL) break;

        rdlock_node(cur_node);

        Node* next_node = cur_node->next;

        int local_less_count = 0;

        while (next_node != NULL) {
            rdlock_node(next_node);
            if (strlen(cur_node->value) < strlen(next_node->value)) {
                local_less_count++;
            }
            unlock_node(cur_node);
            cur_node = next_node;
            next_node = cur_node->next;
        }

        unlock_node(cur_node);
        less_counter_iteration++;
        less_counter_string = local_less_count;
    }

	return NULL;
}

void* equal_cmp(void *arg) {
    set_cpu(2);

    Storage* storage = (Storage*)arg;
	printf("equal_cmp [%d %d %d]\n", getpid(), getppid(), gettid());

    while (1) {
        Node* cur_node = storage->first;

        if (cur_node == NULL) break;

        rdlock_node(cur_node);

        Node* next_node = cur_node->next;

        int local_eq_count = 0;

        while (next_node != NULL) {
            rdlock_node(next_node);
            if (strlen(cur_node->value) == strlen(next_node->value)) {
                local_eq_count++;
            }
            unlock_node(cur_node);
            cur_node = next_node;
            next_node = cur_node->next;
        }
        unlock_node(cur_node);
        equal_counter_iteration++;
        equal_counter_string = local_eq_count;
    }

	return NULL;
}

int maybe_swap(Node* cur_node, Node* next_node, Node* next_next_node, Node* next_next_next_node) {
    if (rand() % 2) {
        cur_node->next = next_next_node;
        next_node->next = next_next_next_node;
        next_next_node->next = next_node;
        return 1;
    }
    return 0;
}

void* swap_cmp(void* arg) {
    set_cpu(3);

    Storage* storage = (Storage*)arg;
    printf("swap_cmp [%d %d %d]\n", getpid(), getppid(), gettid());
    
    while (1) {
        if (storage->first == NULL) break;

        int local_swap_counter = 0;

        Node* cur_node = storage->first;

        if (cur_node == NULL) break;
        wrlock_node(cur_node);

        Node* next_node = cur_node->next;

        while (next_node != NULL) {
            wrlock_node(next_node);
            
            if (next_node->next == NULL) {
                unlock_node(cur_node);
                cur_node = next_node;
                next_node = cur_node->next;
                continue;
            }

            Node* next_next_node = next_node->next;
            wrlock_node(next_next_node);

            int was_swap = maybe_swap(cur_node, next_node, next_next_node, next_next_node->next);
            if (was_swap) {
                next_next_node = next_node;
                next_node = cur_node->next;
            }

            local_swap_counter += was_swap;

            unlock_node(next_next_node);
            unlock_node(cur_node);

            cur_node = next_node;
            next_node = cur_node->next;
        }
        unlock_node(cur_node);

        pthread_mutex_lock(&mutex);
        swap_counter_iteration++;
        swap_counter = local_swap_counter;
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

void *print_stats(void *arg) {
	while (1) {
		printf("stats: greater_counter_iter %d;   greater_counter_str: %d\n less_counter_iter %d;   less_counter_str %d\n equal_counter_iter %d;   equal_counter_str %d\n",
            greater_counter_iteration, greater_counter_string, less_counter_iteration, less_counter_string,
            equal_counter_iteration, equal_counter_string);

        pthread_mutex_lock(&mutex);
        printf("swap_iter_count %d;  swap_counter %d \n\n", swap_counter_iteration, swap_counter);
        pthread_mutex_unlock(&mutex);
		sleep(1);
	}

	return NULL;
}

int main() {
    srand(time(NULL));

    int err;

    Storage* storage = create_storage();

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE);

    err = pthread_mutex_init(&mutex, &attr);
	pthread_mutexattr_destroy(&attr);
    if (err) {
        printf("queue_init: pthread_mutex_init() failed: %s\n", strerror(err));
        abort();
    }

    pthread_t tid_greater;
	err = pthread_create(&tid_greater, NULL, greater_cmp, storage);
	if (err) {
		printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}

    pthread_t tid_less;
	err = pthread_create(&tid_less, NULL, less_cmp, storage);
	if (err) {
		printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}

    pthread_t tid_equal;
	err = pthread_create(&tid_equal, NULL, equal_cmp, storage);
	if (err) {
		printf("main: pthread_create() failed: %s\n", strerror(err));
		return -1;
	}

    pthread_t tids_swap[3];
    for (unsigned int i = 0; i < 3; i++) {
        err = pthread_create(&(tids_swap[i]), NULL, swap_cmp, storage);
        if (err) {
            printf("main: pthread_create() failed: %s\n", strerror(err));
            return -1;
        }
    }

    pthread_t tid_stat;
    err = pthread_create(&tid_stat, NULL, print_stats, NULL);
	if (err) {
		printf("main: pthread_create() failed: %s\n", strerror(err));
		abort();
	}

    err = pthread_join(tid_greater, NULL);
    if (err) {
        printf("main: pthread_join() failed: %s\n", strerror(err));
        return -1;
    }

    err = pthread_join(tid_less, NULL);
    if (err) {
        printf("main: pthread_join() failed: %s\n", strerror(err));
        return -1;
    }

    err = pthread_join(tid_equal, NULL);
    if (err) {
        printf("main: pthread_join() failed: %s\n", strerror(err));
        return -1;
    }

    for (unsigned int i = 0; i < 3; i++) {
        err = pthread_join(tids_swap[i], NULL);
        if (err) {
            printf("main: pthread_join() failed: %s\n", strerror(err));
            return -1;
        }
    }

    err = pthread_cancel(tid_stat);
    if (err) {
        printf("main: pthread_cancel() failed: %s\n", strerror(err));
    }

	pthread_exit(NULL);
}