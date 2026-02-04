#ifndef NETWORKREQUEST_H
#define NETWORKREQUEST_H

#include <string>
#include <vector>
#include <memory>

#include <curl/curl.h>

#include "NetworkResponse.h"

class NetworkRequest
{
    public:
        std::string URL = "";
    public:
        ~NetworkRequest();

        void AddHeader(const std::string& header);
        void AddHeader(const std::string& header, const std::string& value);
        void AddCookie(const std::string& cookie, const std::string& value);

        NetworkResponse Get();
    private:
        struct curl_slist* _headers = NULL;
        std::vector<std::string> _cookies;
    private:
        std::string GetCookieString();
        static size_t CurlWriteFunction(void* data, size_t size, size_t nmemb, void* clientp);
};

#endif