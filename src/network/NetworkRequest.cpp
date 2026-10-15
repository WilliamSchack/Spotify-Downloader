#include "NetworkRequest.h"

void NetworkRequest::SetHeader(const std::string& header)
{
    _headers = curl_slist_append(_headers, header.c_str());
}

void NetworkRequest::SetHeader(const std::string& header, const std::string& value)
{
    std::string headerString = "";
    if (value.empty()) headerString = header + ";";
    else               headerString = header + ": " + value;

    SetHeader(headerString);
}

void NetworkRequest::SetCookiePath(const std::string& path)
{
    _cookiePath = path;
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

NetworkResponse NetworkRequest::Post(nlohmann::json postData)
{
    SetHeader("Content-Type", "application/json");
    return Post(postData.dump());
}

CURL* NetworkRequest::InitCurl()
{
    if (_globalHandler.GlobalInit != CURLcode::CURLE_OK)
        return nullptr;

    CURL* curl = curl_easy_init();
    if (curl == nullptr)
        return curl;

    curl_easy_setopt(curl, CURLOPT_URL, Url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, _headers);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10L);

    if (!_cookiePath.empty())
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, _cookiePath.c_str());

    return curl;
}

NetworkResponse NetworkRequest::SendRequest(CURL* curl)
{
    NetworkResponse response;

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &CurlWriteFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response.Body);
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip, deflate, br, zstd");

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