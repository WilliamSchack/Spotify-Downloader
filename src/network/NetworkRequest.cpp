#include "NetworkRequest.h"

#include <iostream>

void NetworkRequest::AddHeader(const std::string& header)
{
    _headers = curl_slist_append(_headers, header.c_str());
}

void NetworkRequest::AddHeader(const std::string& header, const std::string& value)
{
    std::string headerString = "";
    if (value.empty()) headerString = header + ";";
    else               headerString = header + ": " + value;

    AddHeader(headerString);
}

void NetworkRequest::AddCookie(const std::string& cookie, const std::string& value)
{
    std::string cookieString = cookie + "=" + value + ";";
    _cookies.push_back(cookieString);
}

std::string NetworkRequest::GetCookieString()
{
    std::string cookieString = "";

    int cookiesSize = _cookies.size();
    for (int i = 0; i < cookiesSize; i++) {
        cookieString += _cookies[i];

        if (i < cookiesSize - 1)
            cookieString += " ";
    }

    return cookieString;
}

CURLcode NetworkRequest::Get()
{
    CURLcode globalInit = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (globalInit != CURLE_OK)
        return globalInit;

    CURL* curl = curl_easy_init();
    if (!curl) {
        curl_global_cleanup();
        return globalInit;
    }

    // Set params
    curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, _headers);

    // Request
    CURLcode response = curl_easy_perform(curl);
    std::cout << response << std::endl;

    // Cleanup
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return response;
}

NetworkRequest::~NetworkRequest()
{
    curl_slist_free_all(_headers);
}