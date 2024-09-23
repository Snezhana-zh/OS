#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>
#include <errno.h>

#define PAGE_SIZE 4096
#define NUM_PAGES 10

void signal_handler(int signal) {
    if (signal == SIGSEGV) {
        printf("SIGSEGV\n");
        exit(EXIT_FAILURE);
    }
}

void array_on_stack(int count) {
    if (count > 0) {
        int buffer[4096];
        sleep(3);
        array_on_stack(count - 1);
    }
}

void array_on_heap(int count) {
    for (int i = 0; i < count; i++) {
        printf("%d\n", i);
        char* buf = malloc(1024 * 1024);
        sleep(3);
    }
}

int main() {
    int pid = getpid();
    printf("PID: %d\n", pid);

    sleep(7);

    // array_on_stack(12);

    // array_on_heap(8);


    void* region = mmap(NULL, NUM_PAGES * PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (region == MAP_FAILED) {
        perror("error mapping\n");
        return errno;
    }
    sleep(2);


    if (mprotect(region, NUM_PAGES * PAGE_SIZE, PROT_NONE) == -1) {
        perror("mprotect");
        return errno;
    }
    sleep(2);


    if (signal(SIGSEGV, signal_handler) == SIG_ERR) {
        perror("signal error");
        return errno;
    };

    // int value = *((int*)region);

    // *((int*)region) = 123;

    if (munmap(region + 3 * PAGE_SIZE, 3 * PAGE_SIZE) == -1) {
        perror("munmap");
        return errno;
    }
    sleep(5);
    return 0;
}