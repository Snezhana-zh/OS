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
    struct sockaddr_in clt_socksaddr;
    int err;
    char buff[128];

    srv_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (srv_sock == - 1) {
        printf("socket() failed %s\n", strerror(errno));
        exit(1);
    }

    int port = 2222;
    char* ip = "127.0.0.1";

    memset(&srv_sockaddr, 0, sizeof(struct sockaddr_in));
    srv_sockaddr.sin_family = AF_INET;
    srv_sockaddr.sin_port = htons(port);
    srv_sockaddr.sin_addr.s_addr = inet_addr(ip);

    err = bind(srv_sock, (struct sockaddr*)&srv_sockaddr, sizeof(srv_sockaddr));
    if (err == -1) {
        printf("bind() failed: %s\n", strerror(errno));
        close(srv_sock);
        exit(1);
    }

    while (1) {
        socklen_t len = sizeof(clt_socksaddr);
        printf("Waiting...\n"); 
        err = recvfrom(srv_sock, buff, sizeof(buff), 0, (struct sockaddr*)&clt_socksaddr, &len);
        if (err == -1) {
            printf("recvfrom() failed: %s\n", strerror(errno));
            close(srv_sock);
            exit(1);
        }
        printf("Received buf: %s\n", buff);
        err = sendto(srv_sock, buff, sizeof(buff), 0, (struct sockaddr*)&clt_socksaddr, len);
        if (err == -1) {
            printf("sendto() failed: %s\n", strerror(errno));
            close(srv_sock);
            exit(1);
        }
    }
}