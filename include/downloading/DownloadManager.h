#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include "PlatformDetector.h"
#include "PlatformFactory.h"

#include <string>

class DownloadManager {
    public:
        static bool Download(const std::string& url, const std::string& directory);
};

#endif