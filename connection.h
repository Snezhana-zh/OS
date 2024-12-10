#include <exception>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <string>
#include <iostream>
#include <sstream>
#include <memory>
#include "cache.h"

#define BUFFER_SIZE 1024

class Connection {
public:
    virtual void start() = 0;

    virtual ~Connection() {};

    static void* thread_func(void* arg) {
        try {
            Connection* conn = static_cast<Connection*>(arg);
            conn->start();
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
        return NULL;
    }
};

class ClientConnection : public Connection {
public:
    ClientConnection(int fd, std::shared_ptr<Cache> cache);

    void start();

    ~ClientConnection();
private:
    int client_socket;
    std::shared_ptr<Cache> cache;
};

class ServerConnection : public Connection {
public:
    ServerConnection(std::string url, std::shared_ptr<Cache> cache);

    void start();

    ~ServerConnection();
private:
    std::string url;
    std::shared_ptr<Cache> cache;
};