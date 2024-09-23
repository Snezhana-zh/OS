#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

int global_inited = 10;
int global_not_inited;
const int global_const_inited = 20;
const int global_const_not_inited;

void f() {
    int local_inited = 30;
    int local_not_inited; 
    static int static_inited = 40;
    static int static_not_inited = 50;
    const int const_inited = 60;
    const int const_not_inited;
    
    printf("local_inited %p\nlocal_not_inited %p\nstatic_inited %p\nstatic_not_inited %p\nconst_inited %p\nconst_not_inited %p\n", &local_inited, 
    &local_not_inited, &static_inited, &static_not_inited, &const_inited, &const_not_inited);
}

int* f2() {
    int local_inited_f2 = 30;
    printf("local_inited_f2 %p\n", &local_inited_f2);
    return &local_inited_f2;
}

void signal_handler(int signal) {
    if (signal == SIGSEGV) {
        printf("Page fault");
    }
}

int main() {
    printf("pid: %d\n", getpid());
    printf("global_inited %p\nglobal_not_inited %p\nglobal_const_inited %p\nglobal_const_not_inited %p\n", &global_inited, 
    &global_not_inited, &global_const_inited, &global_const_not_inited);
    f();

    if(signal(SIGSEGV, signal_handler) == SIG_ERR) {
        perror("signal");
        return 1;
    }
    printf("local_inited main %p\n", f2());
    sleep(1000);
}
