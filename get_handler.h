#define CURL_STATICLIB
#include <curl/curl.h>
#include <string>
#include <iostream>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t newLength = size * nmemb;
    try {
        s->append((char*)contents, newLength);
    }
    catch (std::bad_alloc& e) {
        return 0;
    }
    return newLength;
}

std::string performRequest(const std::string& url) {
    CURL* curl;
    CURLcode res;
    std::string bodyBuffer = "";

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L); 

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        curl_easy_setopt(curl, CURLOPT_HEADER, 1L);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &bodyBuffer);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
    } else {
        std::cerr << "curl init() failed" << std::endl;
    }

    return bodyBuffer;
}