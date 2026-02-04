#ifndef IPLATFORMDOWNLOADER_H
#define IPLATFORMDOWNLOADER_H

#include <iostream>
#include <string>
#include <filesystem>

#include "ELinkType.h"

class IPlatformDownloader
{
    public:
        bool Download(const std::string& url, const std::string& directory);
    protected:
        virtual ELinkType GetLinkType(const std::string& url) = 0;
};

#endif