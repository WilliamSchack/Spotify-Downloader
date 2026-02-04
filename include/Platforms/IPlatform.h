#ifndef IPLATFORM_H
#define IPLATFORM_H

#include <string>

class IPlatform
{
    public:
        virtual bool Download(const std::string& url) = 0;
};

#endif