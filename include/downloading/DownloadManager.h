#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include "PlatformDetector.h"
#include "PlatformFactory.h"
#include "TrackDownloader.h"
#include "DownloadProgress.h"
#include "DownloadResult.h"
#include "DownloadEvents.h"

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <atomic>

// Object must be alive for the duration of the download
// otherwise the thread that created it will be blocked until the download finishes
class DownloadManager
{
    public:
        ~DownloadManager();

        // Returns if the download started
        bool Download(const std::string& url, const std::string& directory);

        DownloadEvents& GetEvents();
        bool IsDownloading();
    private:
        void ThreadDownload(const std::vector<TrackData>& tracks, const EPlatform& platformType, const std::string& directory);
    private:
        std::atomic<int> _tracksRemaining = 0;
        std::atomic<int> _failedDownloads = 0;

        DownloadEvents _events;
        std::vector<std::thread> _threads;
};

#endif