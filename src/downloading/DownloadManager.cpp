#include "DownloadManager.h"

DownloadManager::DownloadManager() : _downloadingPlaylist(EPlatform::Unknown) {}

DownloadManager::~DownloadManager()
{
    {
        std::lock_guard<std::mutex> lock(_trackQueueMutex);
        _stopping = true;
    }

    _queueCv.notify_all();

    CleanupThreads();
}

DownloadEventsQueue& DownloadManager::GetEvents()
{
    return _events;
}

void DownloadManager::Pause()
{
    if (!_downloading)
        return;
    
    std::lock_guard<std::mutex> lock(_trackQueueMutex);
    if (_paused)
        return;

    _paused = true;

    if (_unpausedThreads == 0)
        _events.Send(DownloadsPausedEvent());
}

void DownloadManager::Resume()
{
    if (!_downloading)
        return;

    {
        std::lock_guard<std::mutex> lock(_trackQueueMutex);
        _paused = false;
    }

    _queueCv.notify_all();
}

void DownloadManager::Stop()
{
    if (!_downloading)
        return;

    {
        std::lock_guard<std::mutex> lock(_trackQueueMutex);
        _stopping = true;
        _trackQueue.clear();
    }

    _queueCv.notify_all();
}

bool DownloadManager::IsDownloading() const
{
    return _downloading;
}

bool DownloadManager::IsPausing() const
{
    return _downloading && _paused && _unpausedThreads != 0;
}

bool DownloadManager::IsPaused() const
{
    return _downloading && _paused && _unpausedThreads == 0;
}

bool DownloadManager::IsStopping() const
{
    return _downloading && _stopping;
}

int DownloadManager::GetSuccessfulDownloads() const
{
    return _successfulDownloads;
}

int DownloadManager::GetFailedDownloads() const
{
    return _failedDownloads;
}

int DownloadManager::GetCompletedDownloads() const
{
    return _successfulDownloads + _failedDownloads;
}

int DownloadManager::GetTotalTracks() const
{
    return _totalTracks;
}

PlaylistData DownloadManager::GetPlaylistData() const
{
    return _downloadingPlaylist;
}

bool DownloadManager::RequestDownload(const std::string& url, const std::string& directory)
{
    if (_downloading) {
        Logger::Info("This DownloadManager is already downloading, use another or wait for this to finish to start a new download");
        return false;
    }

    _downloading = true;
    CleanupThreads();

    _creatorThread = std::thread([this, url, directory]() {
        StartDownload(url, directory);
    });

    return true;
}

void DownloadManager::StartDownload(const std::string& url, const std::string& directory)
{
    std::string directoryValidated = FileUtils::ValidateDirectory(directory);
    if (!std::filesystem::exists(FileUtils::PathFromUtf8(directoryValidated))) {
        DownloadFailedToStartEvent event;
        event.Reason = "The chosen directory is invalid";
        _events.Send(event);
        _downloading = false;
        return;
    }

    EPlatform platform = PlatformDetector::GetPlatformFromUrl(url);
    if (platform == EPlatform::Unknown) {
        DownloadFailedToStartEvent event;
        event.Reason = "This platform is unknown, please enter an implemented platform";
        _events.Send(event);
        _downloading = false;
        return;
    }

    std::unique_ptr<IPlatformDownloader> platformDownloader = PlatformFactory::CreateDownloader(platform);
    if (platformDownloader == nullptr) {
        DownloadFailedToStartEvent event;
        event.Reason = "I forgot to implement this platform... please inform me of this, thanks :)";
        _events.Send(event);
        _downloading = false;
        return;
    }

    EPlatform searchPlatform = platformDownloader->GetSearchPlatform();
    if (searchPlatform == EPlatform::Unknown) {
        DownloadFailedToStartEvent event;
        event.Reason = "I forgot to add a search platform to this platform... please inform me of this, thanks :)";
        _events.Send(event);
        _downloading = false;
        return;
    }

    // Get the tracks
    ELinkType linkType = platformDownloader->GetLinkType(url);
    if (linkType == ELinkType::Unknown) {
        DownloadFailedToStartEvent event;
        event.Reason = "Could not get the link type, please send this link to me so I can add it";
        _events.Send(event);
        _downloading = false;
        return;
    }

    std::vector<TrackData> tracks;
    switch (linkType) {
        case ELinkType::Track: {
            TrackData track = platformDownloader->GetTrack(url);
            tracks.push_back(track);

            PlaylistData playlistData(platform);
            playlistData.Id = track.Id;
            playlistData.Name = track.Name;
            playlistData.Owner = track.GetMainArtist();
            playlistData.TotalTracks = 1;
            playlistData.ImageUrl = track.Album.ImageUrl;
            _downloadingPlaylist = playlistData;

            break;
        } case ELinkType::Playlist: {
            PlaylistTracks playlist = platformDownloader->GetPlaylist(url);
            _downloadingPlaylist = playlist.Data;
            tracks = playlist.Tracks;
            break;
        } case ELinkType::Album: {
            AlbumTracks album = platformDownloader->GetAlbum(url);
            tracks = album.Tracks;

            PlaylistData playlistData(platform);
            playlistData.Id = album.Data.Id;
            playlistData.Name = album.Data.Name;
            playlistData.Owner = album.Data.GetMainArtist();
            playlistData.TotalTracks = album.Tracks.size();
            playlistData.ImageUrl = album.Data.ImageUrl;
            _downloadingPlaylist = playlistData;

            break;
        } default:
            DownloadFailedToStartEvent event;
            event.Reason = "I forgot to implement this link type... please inform me of this, thanks :)";
            _events.Send(event);
            _downloading = false;
            return;
    }

    if (tracks.size() == 0){
        DownloadFailedToStartEvent event;
        event.Reason = "This link returned 0 tracks";
        _events.Send(event);
        _downloading = false;
        return;
    }

    // Setup
    _failedDownloads = 0;
    _successfulDownloads = 0;
    _totalTracks = tracks.size();
    _downloadingDir = directoryValidated;
    _downloadedFilePaths.clear();
    _trackQueue = std::deque<TrackData>(tracks.begin(), tracks.end());

    DownloadStartedEvent startedEvent;
    startedEvent.Tracks = tracks;
    _events.Send(startedEvent);

    // Dispatch threads
    int threadCount = std::min<int>(tracks.size(), Config::PerDownloadThreads);
    _threadsDownloading = threadCount;

    {
        std::lock_guard<std::mutex> lock(_trackQueueMutex);
        _unpausedThreads = threadCount;
    }

    Logger::Info("Starting download for (" + url + ") with (" + std::to_string(tracks.size()) + ") tracks, outputting to (" + directoryValidated + ")");
    Logger::Flush();

    for (int i = 0; i < threadCount; i++) {
        _threads.emplace_back([this, searchPlatform, directoryValidated]() {
            ThreadDownload(searchPlatform, directoryValidated);
        });
    }
}

void DownloadManager::ThreadDownload(const EPlatform& searchPlatform, const std::string& directory)
{
    std::ostringstream threadSs;
    threadSs << std::this_thread::get_id();
    Logger::Info("Starting download on thread: " + threadSs.str());

    while (std::optional<TrackData> track = GetNextTrack()) {
        if (track->Id.empty()) {
            TrackFailedEvent event;
            event.TrackUniqueId = track->GetUniqueId();
            event.Reason = "Could not find the id for this track";
            _events.Send(event);

            _failedDownloads++;

            Logger::Warning("Track (" + event.TrackUniqueId + ") failed to download with the error: " + event.Reason);

            continue;
        }

        DownloadResult result = TrackDownloader::DownloadTrack(*track, searchPlatform, directory, [&](DownloadEvent event) {
            _events.Send(event);
        });

        if (result.Success) {
            {
                std::lock_guard<std::mutex> lock(_downloadedFilePathsMutex);
                _downloadedFilePaths.push_back(result.FilePath);
            }

            TrackSucceededEvent event;
            event.TrackUniqueId = track->GetUniqueId();
            _events.Send(event);

            _successfulDownloads++;
        } else {
            TrackFailedEvent event;
            event.TrackUniqueId = track->GetUniqueId();
            event.Reason = result.FailReason;
            _events.Send(event);

            _failedDownloads++;
            
            Logger::Warning("Track (" + event.TrackUniqueId + ") failed to download with the error: " + event.Reason);
        }
        
        // Keep the log file up to date incase of a crash
        Logger::Flush();
    }

    if (--_threadsDownloading == 0)
        FinishDownload();

    Logger::Info("Thread finished downloading: " + threadSs.str());
}

std::optional<TrackData> DownloadManager::GetNextTrack()
{
    std::unique_lock<std::mutex> lock(_trackQueueMutex);

    if (_paused && !_stopping) {
        if (--_unpausedThreads == 0)
            _events.Send(DownloadsPausedEvent());

        _queueCv.wait(lock, [this] { return !_paused || _stopping; });
        _unpausedThreads++;
    }

    if (_stopping || _trackQueue.empty()) {
        _unpausedThreads--;
        return std::nullopt;
    }

    TrackData track = std::move(_trackQueue.front());
    _trackQueue.pop_front();
    return track;
}

void DownloadManager::CreatePlaylistFile()
{
    // Get file path
    SavablePlaylistTagHandler tagHandler(_downloadingPlaylist);
    tagHandler.SetSaveLocation(_downloadingDir);
    TagHandlerResult tagResult = tagHandler.FormatString(Config::PlaylistFileName, Config::PlaylistFileNameTagsOpeningChar, Config::PlaylistFileNameTagsClosingChar);
    if (tagResult.Error != ETagError::None) {
        // TODO: Add event for failed playlist file (if needed)
        Logger::Warning("Could not get the playlist file name with the error: " + tagResult.ErrorString);
        return;
    }

    // Create playlist file
    std::unique_ptr<IPlaylistFile> playlistFile = PlaylistFileFactory::Create(Config::PlaylistFileType);
    if (playlistFile == nullptr) {
        // TODO: Add event for failed playlist file (if needed)
        Logger::Warning("I forgot to implement the playlist file type (" + std::to_string((int)Config::PlaylistFileType) + ")... please inform me of this, thanks :)");
        return;
    }
    
    std::string filePathString = FileUtils::ValidateDirectory(tagResult.FormattedString);
    playlistFile->CreatePlaylistFileFromTracks(_downloadedFilePaths, filePathString);
}

void DownloadManager::FinishDownload()
{
    if (_stopping) {
        Logger::Info("Downloads cancelled: Successes (" + std::to_string(_successfulDownloads) + "), Failures (" + std::to_string(_failedDownloads) + ") / " + std::to_string(_totalTracks));

        DownloadsCancelledEvent event;
        event.FailedDownloads = _failedDownloads;
        event.SuccessfulDownloads = _successfulDownloads;
        event.OutputFolder = FileUtils::PathToUtf8(_downloadingDir);
        _events.Send(event);

        _downloading = false;
        _stopping = false;

        Logger::Flush();

        return;
    }

    Logger::Info("Finished all downloads: Successes (" + std::to_string(_successfulDownloads) + "), Failures (" + std::to_string(_failedDownloads) + "), Total (" + std::to_string(_totalTracks) + ")");

    // Create playlist file if required
    if (Config::CreatePlaylistFile && Config::PlaylistFileType != EPlaylistFileType::None)
        CreatePlaylistFile();

    // Send event
    DownloadsFinishedEvent event;
    event.FailedDownloads = _failedDownloads;
    event.SuccessfulDownloads = _successfulDownloads;
    event.OutputFolder = FileUtils::PathToUtf8(_downloadingDir);
    _events.Send(event);

    _downloading = false;

    Logger::Flush();
}

void DownloadManager::CleanupThreads()
{
    if (_creatorThread.joinable())
        _creatorThread.join();

    for (std::thread& thread : _threads) {
        if (thread.joinable())
            thread.join();
    }
    _threads.clear();

    _events.ClearAll();

    {
        std::lock_guard<std::mutex> lock(_trackQueueMutex);
        _paused = false;
        _stopping = false;
    }
}