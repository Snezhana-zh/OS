#ifndef CACHE_H
#define CACHE_H

#include <map>
#include <iostream>
#include <string>
#include <pthread.h>
#include <exception>
#include <errno.h>
#include <list>
#include <queue>
#include <chrono>

#define QUEUE_SIZE 64

class CacheItem {
private:
    std::string data;
    std::string key;
    std::chrono::time_point<std::chrono::steady_clock> createdTime;
    std::chrono::minutes ttl{1};
public:
    CacheItem(std::string key, std::string data);
    std::string& getData();
    bool timeOut(std::chrono::time_point<std::chrono::steady_clock> time);
};

class Cache {
public:
    Cache();

    void put(std::string key, std::string data);

    std::string get(std::string key);

    std::pair<std::string, bool> hasElem(std::string key);

    void clean();

    ~Cache();
private:
    std::map<std::string, CacheItem*> cache_map;

    pthread_rwlock_t lock; // erase don't thread safety

    // pthread_mutex_t mutex;
};

#endif