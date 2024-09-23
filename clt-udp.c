#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <unistd.h>


int main() {
    int clt_sock;
    struct sockaddr_in srv_sockaddr;
    int err;
    int count_bytes;
    char buff_send[128];
    char buff_recv[128];

    clt_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (clt_sock == - 1) {
        printf("socket() failed %s\n", strerror(errno));
        exit(1);
    }

    int port = 2222;
    char* ip = "127.0.0.1";

    memset(&srv_sockaddr, 0, sizeof(struct sockaddr_un));
    srv_sockaddr.sin_family = AF_INET;
    srv_sockaddr.sin_port = htons(port);
    srv_sockaddr.sin_addr.s_addr = inet_addr(ip);

    while (1) {
        printf("Enter message...\n");

        fgets(buff_send, sizeof(buff_send), stdin);

        err = sendto(clt_sock, buff_send, sizeof(buff_send), 0, (struct sockaddr*)&srv_sockaddr, sizeof(srv_sockaddr));
        if (err == -1) {
            printf("sendto() failed: %s\n", strerror(errno));
            close(clt_sock);
            exit(1);
        }

        memset(buff_recv, 0, sizeof(buff_recv));

        err = recvfrom(clt_sock, buff_recv, sizeof(buff_recv), 0, NULL, NULL);
        if (err == -1) {
            printf("recvfrom() failed: %s\n", strerror(errno));
            close(clt_sock);
            exit(1);
        }
        printf("Received message: %s\n", buff_recv);
    }
}