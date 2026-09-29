#include "DownloadManager.h"

// debugging
#include <iostream>

DownloadManager::~DownloadManager()
{
    CleanupThreads();
}

DownloadEventsQueue& DownloadManager::GetEvents()
{
    return _events;
}

bool DownloadManager::IsDownloading()
{
    return _downloading;
}

bool DownloadManager::RequestDownload(const std::string& url, const std::string& directory)
{
    if (_downloading) {
        std::cout << "This DownloadManager is already downloading, use another or wait for this to finish to start a new download" << std::endl;
        return false;
    }

    CleanupThreads();
    _downloading = true;
    _events.ClearAll();

    _threads.emplace_back([this, url, directory]() {
        StartDownload(url, directory);
    });

    return true;
}

void DownloadManager::StartDownload(const std::string& url, const std::string& directory)
{
    bool directoryValid = std::filesystem::exists(directory);
    if (!directoryValid) {
        _downloading = false;
        return;
    }

    EPlatform platformType = PlatformDetector::GetPlatformFromUrl(url);
    if (platformType == EPlatform::Unknown) {
        _downloading = false;
        return;
    }

    std::unique_ptr<IPlatformDownloader> platform = PlatformFactory::CreateDownloader(platformType);
    if (platform == nullptr) {
        _downloading = false;
        return;
    }

    EPlatform searchPlatform = platform->GetSearchPlatform();
    if (searchPlatform == EPlatform::Unknown) {
        _downloading = false;
        return;
    }

    // Get the tracks
    ELinkType linkType = platform->GetLinkType(url);
    if (linkType == ELinkType::Unknown) {
        _downloading = false;
        return;
    }

    std::vector<TrackData> tracks;
    switch (linkType) {
        case ELinkType::Track:
            tracks.push_back(platform->GetTrack(url));
            break;
        case ELinkType::Playlist:
            tracks = platform->GetPlaylist(url).Tracks;
            break;
        case ELinkType::Album:
            tracks = platform->GetAlbum(url).Tracks;
            break;
        default:
            _downloading = false;
            return;
    }

    std::cout << tracks.size() << std::endl;

    if (tracks.size() == 0){
        _downloading = false;
        return;
    }

    // Get track distribution
    int songCount = tracks.size();
    int threadCount = std::min<int>(songCount, Config::PER_DOWNLOAD_THREADS);

    int baseSongCount = songCount / threadCount;
    int songsRemainder = songCount % threadCount;

    // Setup
    _tracksRemaining = songCount;
    _failedDownloads = 0;

    // Dispatch threads
    int currentStartIndex = 0;
    for (int i = 0; i < threadCount; i++) {
        int currentSongCount = songsRemainder == 0 ? baseSongCount : baseSongCount + 1;
        if (songsRemainder > 0) songsRemainder--;

        std::vector<TrackData> threadTracks(tracks.begin() + currentStartIndex, tracks.begin() + currentStartIndex + currentSongCount);

        _threads.emplace_back([this, threadTracks = std::move(threadTracks), searchPlatform, directory]() {
            ThreadDownload(threadTracks, searchPlatform, directory);
        });

        currentStartIndex += currentSongCount;
    }

    // Have a callback when the threads are finished
    // Could wait here but it would block the main thread
}

void DownloadManager::ThreadDownload(const std::vector<TrackData>& tracks, const EPlatform& searchPlatform, const std::string& directory)
{
    std::cout << "THREAD: " << std::this_thread::get_id() << std::endl;

    for (const TrackData& track : tracks) {
        DownloadResult result = TrackDownloader::DownloadTrack(track, searchPlatform, directory, [&](DownloadProgress p) {
            TrackProgressEvent event;
            event.TrackUniqueId = track.GetUniqueId();
            event.Progress = p.Progress;
            event.Message = p.Message;
            _events.Send(event);
        });

        if (result.Success) {
            TrackSucceededEvent event;
            event.TrackUniqueId = track.GetUniqueId();
            _events.Send(event);
        } else {
            TrackFailedEvent event;
            event.TrackUniqueId = track.GetUniqueId();
            event.Reason = "";
            _events.Send(event);
        }

        if (!result.Success)
            _failedDownloads++;

        if (--_tracksRemaining == 0) {
            // Download is finished
            DownloadsFinishedEvent event;
            _events.Send(event);
            _downloading = false;
        }
    }

    std::cout << "THREAD: " << std::this_thread::get_id() << " FINISHED DOWNLOADING" << std::endl;

    // Figure out thread redistribution
}

void DownloadManager::CleanupThreads()
{
    for (std::thread& thread : _threads) {
        if (thread.joinable())
            thread.join();
    }

    _threads.clear();
}