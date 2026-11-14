#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include "PlatformDetector.h"
#include "PlatformFactory.h"
#include "TrackDownloader.h"
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
        void Stop();

        bool IsDownloading() const;
        bool IsPausing() const;
        bool IsPaused() const;
        bool IsStopping() const;

        PlaylistData GetPlaylistData() const;

        DownloadEventsQueue& GetEvents();
    private:
        void StartDownload(const std::string& url, const std::string& directory);
        void ThreadDownload(const EPlatform& searchPlatform, const std::string& directory);
        std::optional<TrackData> GetNextTrack();

        void CreatePlaylistFile();
        void FinishDownload();
        void CleanupThreads();
    private:
        std::atomic<bool> _downloading = false;
        std::atomic<int> _threadsDownloading = 0;
        std::atomic<int> _failedDownloads = 0;
        std::atomic<int> _successfulDownloads = 0;

        bool _paused = false;
        bool _stopping = false;
        int _unpausedThreads = 0;
        std::condition_variable _queueCv;

        std::filesystem::path _downloadingDir;
        PlaylistData _downloadingPlaylist;

        std::deque<TrackData> _trackQueue;
        std::mutex _trackQueueMutex;

        std::vector<std::filesystem::path> _downloadedFilePaths;
        std::mutex _downloadedFilePathsMutex;

        DownloadEventsQueue _events;

        std::thread _creatorThread;
        std::vector<std::thread> _threads;
};

#endif