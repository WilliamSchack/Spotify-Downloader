#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include "PlatformDetector.h"
#include "PlatformFactory.h"
#include "TrackDownloader.h"
#include "DownloadProgress.h"
#include "DownloadResult.h"
#include "DownloadEventsQueue.h"
#include "PlaylistData.h"
#include "PlaylistFileFactory.h"
#include "SavablePlaylistTagHandler.h"

#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <deque>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

// Object must be alive for the duration of the download otherwise the thread that created it will be blocked until the download finishes
// Events must also be watched for download progress and to check if it started or failed
class DownloadManager
{
    public:
        DownloadManager();
        ~DownloadManager();

        // Returns if the download was dispatched
        bool RequestDownload(const std::string& url, const std::string& directory);

        void Pause();
        void Resume();

        bool IsDownloading();
        bool IsPaused();

        DownloadEventsQueue& GetEvents();
    private:
        void StartDownload(const std::string& url, const std::string& directory);
        void ThreadDownload(const EPlatform& platformType, const std::string& directory);
        std::optional<TrackData> GetNextTrack();

        void CreatePlaylistFile();
        void CleanupThreads();
    private:
        std::atomic<bool> _downloading = false;
        std::atomic<int> _tracksRemaining = 0;
        std::atomic<int> _failedDownloads = 0;
        std::atomic<int> _successfulDownloads = 0;

        std::atomic<bool> _paused = false;
        std::atomic<bool> _stopping = false;
        std::condition_variable _queueCv;

        std::filesystem::path _downloadingDir;
        PlaylistData _downloadingPlaylist;

        std::deque<TrackData> _trackQueue;
        std::mutex _trackQueueMutex;

        std::vector<std::filesystem::path> _downloadedFilePaths;
        std::mutex _downloadedFilePathsMutex;

        DownloadEventsQueue _events;
        std::vector<std::thread> _threads;
};

#endif