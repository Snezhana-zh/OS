#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        printf("child PID = %d\n", getpid());
        sleep(50);
    } else {
        printf("parent PID = %d\n", getpid());
        sleep(20);
        printf("end");
        exit(EXIT_SUCCESS);
    }
}