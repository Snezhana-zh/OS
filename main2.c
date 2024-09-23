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
    int pipe_arr[2];

    if (pipe(pipe_arr) == -1) {
        perror("PIPE");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid == 0) {
        close(pipe_arr[0]);
        unsigned int number = 0;
        while(true) {
            // printf("NUMBER: %u\n", number);
            write(pipe_arr[1], &number, sizeof(number));
            number++;
            if (number >= PAGE_SIZE / sizeof(number)) {
                number = 0;
            }
        }
        close(pipe_arr[1]);
    } else if (pid > 0) {
        close(pipe_arr[1]);
        unsigned int previous_num = 0;
        unsigned int number = 0;
        ssize_t count_read;
        while(true) {
            count_read = read(pipe_arr[0], &number, sizeof(number));
            if (count_read == 0) {
                printf("END OF FILE");
                break;
            }
            if (count_read == -1) {
                perror("READING ERROR");
                exit(EXIT_FAILURE);
            }
            if (number != previous_num) {
                printf("Sequence was broken: (exp)%u - (real)%u\n", previous_num, number);
            }
            else {
                // printf("OK: (exp)%u - (real)%u\n", previous_num, number);
            }
            previous_num++;
            if (previous_num >= PAGE_SIZE / sizeof(number)) {
                previous_num = 0;
            }
        }
        close(pipe_arr[0]);

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
