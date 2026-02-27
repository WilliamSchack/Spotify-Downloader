#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include "PlatformDetector.h"
#include "PlatformFactory.h"

#include <iostream>
#include <string>
#include <vector>
#include <thread>

class DownloadManager {
    public:
        static bool Download(const std::string& url, const std::string& directory);
    private:
        static void ThreadDownload(const std::vector<TrackData>& tracks, const EPlatform& platformType, const std::string& directory);
};

#endif