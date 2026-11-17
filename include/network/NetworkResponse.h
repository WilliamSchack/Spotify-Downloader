#ifndef NETWORKRESPONSE_H
#define NETWORKRESPONSE_H

#include <string>
#include <curl/curl.h>

class NetworkResponse
{
    public:
        unsigned short HTTPCode = 0;
        CURLcode CurlCode = CURLcode::CURLE_OK;
        std::string Body = "";
};

#endif