#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int main() {
    int srv_sock;
    struct sockaddr_in srv_sockaddr;
    int err;
    char buff[128];

    srv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (srv_sock == - 1) {
        printf("socket() failed %s\n", strerror(errno));
        exit(1);
    }

    int port = 2222;
    char* ip = "127.0.0.1";

    memset(&srv_sockaddr, 0, sizeof(struct sockaddr_un));
    srv_sockaddr.sin_family = AF_INET;
    srv_sockaddr.sin_port = htons(port);
    srv_sockaddr.sin_addr.s_addr = inet_addr(ip);

    err = bind(srv_sock, (struct sockaddr*)&srv_sockaddr, sizeof(srv_sockaddr));
    if (err == -1) {
        printf("bind() failed: %s\n", strerror(errno));
        close(srv_sock);
        exit(1);
    }

    err = listen(srv_sock, 5);
    if (err == -1) {
        printf("listen() failed: %s\n", strerror(errno));
        close(srv_sock);
        exit(1);
    }

    int clt_sock;

    while(1) {
        int pid;
        struct sockaddr_in clt_socksaddr;
        socklen_t len = sizeof(clt_socksaddr);

        printf("Waiting connection...\n");

        memset(&clt_socksaddr, 0, sizeof(struct sockaddr_in));

        clt_sock = accept(srv_sock, (struct sockaddr*)&clt_socksaddr, &len);
        if (clt_sock == -1) {
            printf("accept() failed: %s\n", strerror(errno));
            close(srv_sock);
            exit(1);
        }

        printf("New client accepted!\n");

        pid = fork();
        if (pid == 0) break;
        close(clt_sock);
    }
    close(srv_sock);
    while (1) {
        printf("Waiting...\n"); 
        err = recv(clt_sock, buff, sizeof(buff), 0);
        if (err == -1) {
            printf("recv() failed: %s\n", strerror(errno));
            exit(1);
        }
        printf("Received buf: %s\n", buff);
        err = send(clt_sock, buff, sizeof(buff), 0);
        if (err == -1) {
            printf("send() failed: %s\n", strerror(errno));
            exit(1);
        }
    }
}