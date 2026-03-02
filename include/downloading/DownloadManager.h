#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include "PlatformDetector.h"
#include "PlatformFactory.h"
#include "TrackDownloader.h"

#include <iostream>
#include <string>
#include <vector>
#include <thread>

class DownloadManager {
    public:
        // Returns if the download started
        static bool Download(const std::string& url, const std::string& directory);
    private:
        static void ThreadDownload(const std::vector<TrackData>& tracks, const EPlatform& platformType, const std::string& directory);
};

#endif