#ifndef NETWORKREQUEST_H
#define NETWORKREQUEST_H

#include <string>
#include <vector>
#include <memory>

#include <curl/curl.h>

class NetworkRequest
{
    public:
        std::string URL = "";
    public:
        ~NetworkRequest();

        void AddHeader(const std::string& header);
        void AddHeader(const std::string& header, const std::string& value);
        void AddCookie(const std::string& cookie, const std::string& value);

        CURLcode Get();
    private:
        struct curl_slist* _headers = NULL;
        std::vector<std::string> _cookies;
    private:
        std::string GetCookieString();
};

#endif