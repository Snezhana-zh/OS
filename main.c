#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int global_var = 10;

int main() {
    int local_var = 20;

    printf("global var address: %p\n", &global_var);
    printf("local var address: %p\n", &local_var);
    printf("global var value: %d\n", global_var);
    printf("local var value: %d\n", local_var);

    printf("PID: %d\n", getpid());

    pid_t pid = fork();

    if (pid == 0) {
        printf("child PID: %d\n", getpid());
        printf("parent PID: %d\n", getppid());
        
        printf("child global var address: %p\n", &global_var);
        printf("child local var address: %p\n", &local_var);
        printf("child global var value: %d\n", global_var);
        printf("child local var value: %d\n", local_var);

        sleep(30);

        global_var = 30;
        local_var = 40;
        sleep(10);
        exit(5);
    } else if (pid > 0) {
        printf("NEW global var value: %d\n", global_var);
        printf("NEW local var value: %d\n", local_var);
        sleep(30);
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
