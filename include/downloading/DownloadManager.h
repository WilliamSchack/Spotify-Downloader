#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include "PlatformDetector.h"
#include "PlatformFactory.h"
#include "TrackDownloader.h"
#include "DownloadProgress.h"
#include "DownloadResult.h"
#include "DownloadEventsQueue.h"

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <atomic>

// Object must be alive for the duration of the download otherwise the thread that created it will be blocked until the download finishes
// Events must also be watched for download progress and to check if it started or failed
class DownloadManager
{
    public:
        ~DownloadManager();

        // Returns if the download was dispatched
        bool RequestDownload(const std::string& url, const std::string& directory);
        bool IsDownloading();

        DownloadEventsQueue& GetEvents();
    private:
        void StartDownload(const std::string& url, const std::string& directory);
        void ThreadDownload(const std::vector<TrackData>& tracks, const EPlatform& platformType, const std::string& directory);
        void CleanupThreads();
    private:
        std::atomic<bool> _downloading = false;
        std::atomic<int> _tracksRemaining = 0;
        std::atomic<int> _failedDownloads = 0;
        std::atomic<int> _successfulDownloads = 0;

        DownloadEventsQueue _events;
        std::vector<std::thread> _threads;
};

#endif