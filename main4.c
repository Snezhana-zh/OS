#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char* argv[]) {
    printf("pid: %d\n", getpid());
    sleep(1);
    if (execve(argv[0], argv, NULL) == -1) {
        perror("Error in execve() function\n");
        return errno;
    }
    printf("HELLO");
}