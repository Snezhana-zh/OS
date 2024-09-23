#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char* buffer = (char*)malloc(100);
    if (buffer == NULL) {
        printf("Failed\n");
        return 0;
    }

    const char* str = "hello world";
    strcpy(buffer, str);

    printf("buf: %s\n", buffer);

    free(buffer);

    printf("buf after free: %s\n", buffer);

    buffer = (char*)malloc(100);
    if (buffer == NULL) {
        printf("Failed\n");
        return 0;
    }

    strcpy(buffer, str);

    printf("buf: %s\n", buffer);

    char* mp = buffer + 50;

    free(mp);

    printf("buf after free: %s\n", buffer);
}
