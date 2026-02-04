#ifndef NETWORKRESPONSE_H
#define NETWORKRESPONSE_H

#include <string>
#include <curl/curl.h>

class NetworkResponse
{
    public:
        int HTTPCode;
        CURLcode CurlCode;
        std::string Body;
};

#endif