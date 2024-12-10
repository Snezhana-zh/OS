#include <stdio.h>
#include "proxy.h"

#define PORT 8080
#define CONNECTION_COUNT 64

int main() {
    try {
        HTTPProxy* srv = new HTTPProxy(PORT, CONNECTION_COUNT);
        srv->run();
    }
    catch (std::exception& e) {
        std::cerr << "Failed: " << e.what() << std::endl;
    }
}

// g++ main.cpp connection.cpp srv_conn.cpp proxy.cpp cache.cpp -lcurl -o main