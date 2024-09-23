#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <stdbool.h>

#define PAGE_SIZE 4096
int main() {
    void* region = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (region == MAP_FAILED) {
        perror("error mapping\n");
        return errno;
    }

    pid_t pid = fork();
    if (pid == 0) {
        unsigned int* data = (unsigned int*)region;
        unsigned int number = 0;
        while(true) {
            *data = number++;
            data++;
            if (data >= (unsigned int*)(region + PAGE_SIZE)) {
                data = region;
                number %= 2048;
            }
        }
    } else if (pid > 0) {
        unsigned int* data = (unsigned int*)region;
        unsigned int previous_num = 0;
        while(true) {
            unsigned int cur_number = *data;
            if (cur_number != previous_num) {
                // printf("Sequence was broken: (exp)%u - (real)%u\n", previous_num, cur_number);

            }
            else {
                printf("OK: (exp)%u - (real)%u\n", previous_num, cur_number);
            }
            data++;
            previous_num++;
            if (data >= (unsigned int*)(region + PAGE_SIZE)) {
                data = region;
                previous_num %= 2048;
            }
        }

        int status;
        if (wait(&status) == -1) {
            perror("error in wait()\n");
            exit(EXIT_FAILURE);
        }
        if (WIFEXITED(status)) {
            printf("child exit status: %d\n", WEXITSTATUS(status));
        } else if(WIFSIGNALED(status)) {
            printf("child signal number: %d\n", WTERMSIG(status));            
        } else if(WIFSTOPPED(status)) {
            printf("child signal number(stop): %d\n", WSTOPSIG(status));               
        } else if(WIFCONTINUED(status)) {
            printf("process was resumed\n"); 
        }
    } else {
        perror("fork failed\n");
        exit(EXIT_FAILURE);
    }
    return 0;

}
