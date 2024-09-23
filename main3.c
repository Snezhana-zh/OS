#include <stdio.h>
#include <stdlib.h>

int main() {
    char* env = getenv("MY_VAR");
    if (env != NULL) {
        printf("Value of MY_VAR: %s\n", env);
    } else {
        printf("Error\n");
    }

    if (setenv("MY_VAR", "15", 1) == -1) {
        //perror("Error\n");
    };

    env = getenv("MY_VAR");
    if (env != NULL) {
        printf("New value of MY_VAR: %s\n", env);
    } else {
        printf("Error\n");
    }
    return 0;
}
