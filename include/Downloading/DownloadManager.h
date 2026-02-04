#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <string>

#include "PlatformDetector.h"
#include "PlatformFactory.h"

class DownloadManager {
    public:
        static bool Download(const std::string& url);
};

#endif