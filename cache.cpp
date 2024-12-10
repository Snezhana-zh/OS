#include "cache.h"

CacheItem::CacheItem(std::string k, std::string d) : key(k), data(d) {
    use_count = 1;
}

std::string& CacheItem::getData() {
    return data;
}

std::string& CacheItem::getKey() {
    return key;
}

int CacheItem::getUseCount() {
    return use_count;
}

Cache::Cache() {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE);

    int err = pthread_mutex_init(&mutex, &attr);
	pthread_mutexattr_destroy(&attr);
    if (err) {
        throw std::runtime_error("pthread_mutex_init() failed");
    }
}

void Cache::put(std::string key, std::string data) {
    pthread_mutex_lock(&mutex);

    if (cache_map.size() > QUEUE_SIZE) {
        auto it = cache_map.begin();
        cache_map.erase(it);
    }

    auto it = cache_map.find(key);

    if (it == cache_map.end()) {
        CacheItem* item = new CacheItem(key, data);
        cache_map[key] = item;
    }

    pthread_mutex_unlock(&mutex);
}

std::string Cache::get(std::string key) {
    pthread_mutex_lock(&mutex);

    std::string data = "";

    auto it = cache_map.find(key);

    if (it != cache_map.end()) {
        data = it->second->getData();
    }

    pthread_mutex_unlock(&mutex);
    return data;
}

std::pair<std::string, bool> Cache::hasElem(std::string key) {
    std::string data = get(key);

    if (data.size() == 0) return std::pair("", false);
    else return std::pair(data, true);
}

Cache::~Cache() {
    int err = pthread_mutex_destroy(&mutex);

    if (err) {
        std::cerr << "pthread_mutex_destroy() failed: " << std::endl;
    }

    for (auto& elem : cache_map) {
        delete elem.second;
    }
    
    cache_map.clear();
}