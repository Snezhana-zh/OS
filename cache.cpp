#include "cache.h"

CacheItem::CacheItem(std::string k, std::string d) : key(k), data(d), createdTime(std::chrono::steady_clock::now()) {
}

std::string& CacheItem::getData() {
    return data;
}

bool CacheItem::timeOut(std::chrono::time_point<std::chrono::steady_clock> time) {
    auto elapsed = std::chrono::duration_cast<std::chrono::minutes>(time - createdTime);

    if (elapsed >= ttl) {
        return true;
    }
    return false;
}

Cache::Cache() {
    // pthread_mutexattr_t attr;
    // pthread_mutexattr_init(&attr);
    // pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE);

    // int err = pthread_mutex_init(&lock, &attr);
	// pthread_mutexattr_destroy(&attr);
    // if (err) {
    //     throw std::runtime_error("pthread_mutex_init() failed");
    // }
    pthread_rwlockattr_t attr;
    int err;

    err = pthread_rwlockattr_init(&attr);
    if (err) throw std::runtime_error("pthread_rwlockattr_init() failed");

    err = pthread_rwlockattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE);
    if (err) throw std::runtime_error("pthread_rwlockattr_setpshared() failed");

    err = pthread_rwlock_init(&lock, &attr);
    if (err) {
        throw std::runtime_error("pthread_rwlock_init() failed");
    }
}

void Cache::clean() {
    if (cache_map.size() > QUEUE_SIZE) {
        for (auto it = cache_map.begin(); it != cache_map.end();) {
            if (it->second->timeOut(std::chrono::steady_clock::now())) {
                it = cache_map.erase(it);
            }
            else {
                it++;
            }
        }
    }

    if (cache_map.size() > QUEUE_SIZE) {
        auto it = cache_map.begin();
        
        cache_map.erase(it);
    }
}

void Cache::put(std::string key, std::string data) {
    pthread_rwlock_wrlock(&lock);

    clean();

    auto it = cache_map.find(key);

    if (it == cache_map.end()) {
        CacheItem* item = new CacheItem(key, data);
        cache_map[key] = item;
    }
    else {
        it->second->getData() = data;
    }

    pthread_rwlock_unlock(&lock);
}

std::string Cache::get(std::string key) {
    pthread_rwlock_rdlock(&lock);

    std::string data = "";

    auto it = cache_map.find(key);

    if (it != cache_map.end()) {
        data = it->second->getData();
    }

    pthread_rwlock_unlock(&lock);
    return data;
}

std::pair<std::string, bool> Cache::hasElem(std::string key) {
    std::string data = get(key);

    if (data.size() == 0) return std::pair("", false);
    else return std::pair(data, true);
}

Cache::~Cache() {
    pthread_rwlock_wrlock(&lock);

    for (auto& elem : cache_map) {
        delete elem.second;
    }
    
    cache_map.clear();

    pthread_rwlock_unlock(&lock);

    int err = pthread_rwlock_destroy(&lock);

    if (err) {
        std::cerr << "pthread_rwlock_destroy() failed: " << std::endl;
    } 
}