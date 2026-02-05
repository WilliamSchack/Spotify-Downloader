#include "NetworkRequest.h"

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

NetworkResponse NetworkRequest::Get()
{
    CURL* curl = InitCurl();
    if (curl == nullptr)
        return NetworkResponse();

    NetworkResponse response = SendRequest(curl);

    // Cleanup
    CleanupCurl(curl);
    return response;
}

NetworkResponse NetworkRequest::Post(const std::string& postData)
{
    CURL* curl = InitCurl();
    if (curl == nullptr)
        return NetworkResponse();

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

    NetworkResponse response = SendRequest(curl);

    CleanupCurl(curl);
    return response;
}

CURL* NetworkRequest::InitCurl()
{
    if (_globalHandler.GlobalInit != CURLcode::CURLE_OK)
        return nullptr;

    CURL* curl = curl_easy_init();
    if (curl == nullptr)
        return curl;

    curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, _headers);

    return curl;
}

NetworkResponse NetworkRequest::SendRequest(CURL* curl)
{
    NetworkResponse response;

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &CurlWriteFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response.Body);

    response.CurlCode = curl_easy_perform(curl);

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.HTTPCode);

    return response;
}

void NetworkRequest::CleanupCurl(CURL* curl)
{
    curl_easy_cleanup(curl);
}

size_t NetworkRequest::CurlWriteFunction(void* data, size_t size, size_t nmemb, void* clientp)
{
    size_t totalSize = size * nmemb;
    std::string* stream = static_cast<std::string*>(clientp);
    stream->append(static_cast<char*>(data), totalSize);
    return totalSize;
}

NetworkRequest::~NetworkRequest()
{
    curl_slist_free_all(_headers);
}