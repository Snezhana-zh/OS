#include "connection.h"
#include "get_handler.h"

ServerConnection::ServerConnection(std::string s, std::shared_ptr<Cache> c) : url(s), cache(c) {

}

void ServerConnection::start() {
    std::string response = performRequest(url);
    cache->put(url, response);
}

ServerConnection::~ServerConnection() {

}