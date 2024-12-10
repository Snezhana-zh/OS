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

#define QUEUE_SIZE 64

class CacheItem {
private:
    int use_count;
    std::string data;
    std::string key;
public:
    CacheItem(std::string key, std::string data);
    std::string& getData();
    std::string& getKey();
    int getUseCount();
};

class Cache {
public:
    Cache();

    void put(std::string key, std::string data);

    std::string get(std::string key);

    std::pair<std::string, bool> hasElem(std::string key);

    ~Cache();
private:
    std::map<std::string, CacheItem*> cache_map;

    pthread_mutex_t mutex;
};

#endif