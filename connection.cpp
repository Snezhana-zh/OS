#include "connection.h"

ClientConnection::ClientConnection(int fd, std::shared_ptr<Cache> c) : client_socket(fd), cache(c) {
    
}

ClientConnection::~ClientConnection() {
    close(client_socket);
}

void ClientConnection::start() {
    char buffer[BUFFER_SIZE];
    int bytes_read;

    if ((bytes_read = read(client_socket, buffer, BUFFER_SIZE)) <= 0) {
        throw std::runtime_error("read socket");
    }

    std::string request(buffer, bytes_read);
    std::string method, url, version;

    std::istringstream iss(request);
    iss >> method >> url >> version;

    std::cerr << "Request: " << method << " " << url << " " << version << std::endl;

    auto elem = cache->hasElem(url);

    if (elem.second) {
        // Если данные есть в кэше, отправляем их клиенту
        std::string data = elem.first;
        size_t count = write(client_socket, data.data(), data.size());
        std::cerr << "DATA CACHE: " << data << std::endl;
        std::cerr << "COUNT BYTES: " << count << std::endl;
    } else {
        // Если данных нет в кэше, создаем новый поток для загрузки данных
        Connection* srv_conn = new ServerConnection(url, cache);
        pthread_t fetch_thread;
        if (pthread_create(&fetch_thread, NULL, Connection::thread_func, (void*)srv_conn) != 0) {
            throw std::runtime_error("pthread_create failed");
        }
        pthread_join(fetch_thread, NULL);
        delete srv_conn;

        auto res = cache->hasElem(url);
        std::string data;
        if (res.second) {
            data = res.first;
        } else {
            data = "Data not found";
        }
        size_t count = write(client_socket, data.data(), data.size());
        std::cerr << "DATA: " << data << std::endl;
        std::cerr << "COUNT BYTES: " << count << std::endl;
    }
}