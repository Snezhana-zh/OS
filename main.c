#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>

#define CLIENTS_COUNT 50

int main() {
    int srv_sock;
    int clt_socks[CLIENTS_COUNT];
    struct sockaddr_in srv_sockaddr;
    struct sockaddr_in clt_socksaddr;
    int len = 0;
    int err;
    char buff[128];
    
    memset(clt_socks, -1, sizeof(clt_socks));

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


    err = bind(srv_sock, (struct sockaddr *) &srv_sockaddr, sizeof(srv_sockaddr));
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

    int index = 0;

    while (1) {
        fd_set rfds;
        struct timeval tv;
        int ret;
        int max_fd;

        FD_ZERO(&rfds);
        FD_SET(srv_sock, &rfds);
        max_fd = srv_sock;

        for (int i = 0; i < CLIENTS_COUNT; i++) {
            if (clt_socks[i] != -1) {
                printf("fd %d\n", clt_socks[i]);
                FD_SET(clt_socks[i],&rfds);
                max_fd = clt_socks[i] > max_fd ? clt_socks[i] : max_fd;
            }
        }
    
        tv.tv_sec = 5;
        tv.tv_usec = 0;
        
        ret = select(max_fd + 1, &rfds, NULL, NULL, &tv);
        if (ret == 0) {
            printf("there was no activity on the sockets for 5 seconds\n" );
            continue;
        } else if (ret > 0) {
            if (FD_ISSET(srv_sock, &rfds)) {
                printf("ready to accept new connection\n");
                memset(&clt_socksaddr, 0, sizeof(struct  sockaddr_in));
                clt_socks[index] = accept(srv_sock, (struct sockaddr*) &clt_socksaddr, &len);
                if (clt_socks[index] == -1) {
                    printf( "accept() failed: %s\n", strerror (errno));
                    close(srv_sock);
                    exit(1);
                }
                index++;
            }
            for (int i = 0; i < CLIENTS_COUNT; i++) {
                if (clt_socks[i] != -1 && FD_ISSET(clt_socks[i], &rfds)) {
                    ret = read(clt_socks[i], buff, sizeof(buff));
                    if (ret == 0) {
                        close(clt_socks[i]);
                        clt_socks[i] = -1;
                        break;
                    }
                    printf("read %s\n", buff);
                    ret = write(clt_socks[i], buff, ret);
                    if (ret == 0) {
                        close(clt_socks[i]);
                        clt_socks[i] = -1;
                        break;
                    }
                }
            }
        } else {
            printf("select() failed: %s\n", strerror(errno));
            continue;
        }
    }
}