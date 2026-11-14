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

void DownloadManager::Cancel()
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

PlaylistData DownloadManager::GetPlaylistData() const
{
    return _downloadingPlaylist;
}

bool DownloadManager::RequestDownload(const std::string& url, const std::string& directory)
{
    if (_downloading) {
        std::cout << "This DownloadManager is already downloading, use another or wait for this to finish to start a new download" << std::endl;
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
    if (!std::filesystem::exists(directoryValidated)) {
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

    for (int i = 0; i < threadCount; i++) {
        _threads.emplace_back([this, searchPlatform, directoryValidated]() {
            ThreadDownload(searchPlatform, directoryValidated);
        });
    }
}

void DownloadManager::ThreadDownload(const EPlatform& searchPlatform, const std::string& directory)
{
    std::cout << "THREAD: " << std::this_thread::get_id() << std::endl;

    while (std::optional<TrackData> track = GetNextTrack()) {
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
            
            std::cout << "Track (" << event.TrackUniqueId << ") failed to download with the error: " << event.Reason << std::endl;
        }
    }

    if (--_threadsDownloading == 0)
        FinishDownload();

    std::cout << "THREAD: " << std::this_thread::get_id() << " FINISHED DOWNLOADING" << std::endl;
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
        std::cout << "Could not get the playlist file name with the error: " + tagResult.ErrorString << std::endl;
        return;
    }

    // Create playlist file
    std::unique_ptr<IPlaylistFile> playlistFile = PlaylistFileFactory::Create(Config::PlaylistFileType);
    if (playlistFile == nullptr) {
        // TODO: Add event for failed playlist file (if needed)
        std::cout << "I forgot to implement the playlist file type (" << (int)Config::PlaylistFileType << ")... please inform me of this, thanks :)";
        return;
    }
    
    std::string filePathString = FileUtils::ValidateDirectory(tagResult.FormattedString);
    playlistFile->CreatePlaylistFileFromTracks(_downloadedFilePaths, filePathString);
}

void DownloadManager::FinishDownload()
{
    if (_stopping) {
        std::cout << "Downloads cancelled" << std::endl;

        DownloadsCancelledEvent event;
        event.FailedDownloads = _failedDownloads;
        event.SuccessfulDownloads = _successfulDownloads;
        event.OutputFolder = FileUtils::PathToUtf8(_downloadingDir);
        _events.Send(event);

        _downloading = false;
        return;
    }

    std::cout << "Finished all the downloads" << std::endl;

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