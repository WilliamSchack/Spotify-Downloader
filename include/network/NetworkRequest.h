#ifndef NETWORKREQUEST_H
#define NETWORKREQUEST_H

#include "NetworkResponse.h"

#include <string>
#include <vector>
#include <memory>

#include <curl/curl.h>

#include <iostream>

class NetworkRequest
{
    public:
    std::string Url = "";
    public:
    ~NetworkRequest();
    
    void AddHeader(const std::string& header);
    void SetHeader(const std::string& header, const std::string& value);
    void AddCookie(const std::string& cookie, const std::string& value);
    
    NetworkResponse Get();
    NetworkResponse Post(const std::string& postData);
    private:
        struct CurlGlobalHandler
        {
            CurlGlobalHandler()  { GlobalInit = curl_global_init(CURL_GLOBAL_DEFAULT); }
            ~CurlGlobalHandler() { curl_global_cleanup(); }
            CURLcode GlobalInit;
        };
    private:
        static inline const CurlGlobalHandler _globalHandler;
        
        struct curl_slist* _headers = NULL;
        std::vector<std::string> _cookies;
    private:
        CURL* InitCurl();
        NetworkResponse SendRequest(CURL* curl);
        void CleanupCurl(CURL* curl);

        std::string GetCookieString();
        static size_t CurlWriteFunction(void* data, size_t size, size_t nmemb, void* clientp);
};

#endif