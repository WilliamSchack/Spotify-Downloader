#include "DownloadManager.h"

// debugging
#include <iostream>

DownloadManager::DownloadManager() : _downloadingPlaylist(EPlatform::Unknown) {}

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

    _downloading = true;
    _events.ClearAll();
    CleanupThreads();

    _threads.emplace_back([this, url, directory]() {
        StartDownload(url, directory);
    });

    return true;
}

void DownloadManager::StartDownload(const std::string& url, const std::string& directory)
{
    bool directoryValid = std::filesystem::exists(directory);
    if (!directoryValid) {
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
            playlistData.Name = track.Name;
            playlistData.Owner = track.GetMainArtist();
            playlistData.TotalTracks = 1;
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
            playlistData.Name = album.Data.Name;
            playlistData.Owner = album.Data.GetMainArtist();
            playlistData.TotalTracks = album.Tracks.size();
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

    // Get track distribution
    int songCount = tracks.size();
    int threadCount = std::min<int>(songCount, Config::PerDownloadThreads);

    int baseSongCount = songCount / threadCount;
    int songsRemainder = songCount % threadCount;

    // Setup
    _tracksRemaining = songCount;
    _failedDownloads = 0;
    _successfulDownloads = 0;
    _downloadingDir = directory;

    DownloadStartedEvent startedEvent;
    startedEvent.Tracks = tracks;
    _events.Send(startedEvent);

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
        }, [&](std::filesystem::path coverArtPath) {
            TrackCoverArtDownloadedEvent event;
            event.TrackUniqueId = track.GetUniqueId();
            event.FilePath = coverArtPath;
            _events.Send(event);
        });

        if (result.Success) {
            TrackSucceededEvent event;
            event.TrackUniqueId = track.GetUniqueId();
            _events.Send(event);

            _successfulDownloads++;
        } else {
            TrackFailedEvent event;
            event.TrackUniqueId = track.GetUniqueId();
            event.Reason = result.FailReason;
            _events.Send(event);

            _failedDownloads++;
        }

        if (--_tracksRemaining == 0) {
            // Download is finished

            // Create playlist file if required
            if (Config::PlaylistFileType != EPlaylistFileType::None)
                CreatePlaylistFile();

            // Send event
            DownloadsFinishedEvent event;
            event.FailedDownloads = _failedDownloads;
            event.SuccessfulDownloads = _successfulDownloads;
            event.OutputFolder = directory;
            _events.Send(event);
            _downloading = false;
        }
    }

    std::cout << "THREAD: " << std::this_thread::get_id() << " FINISHED DOWNLOADING" << std::endl;

    // Figure out thread redistribution
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
    std::string filePathString = FileUtils::ValidateFileName(tagResult.FormattedString);
    std::string filePath = FileUtils::PathFromUtf8(filePathString);

    std::unique_ptr<IPlaylistFile> playlistFile = PlaylistFileFactory::Create(Config::PlaylistFileType);
    if (playlistFile == nullptr) {
        // TODO: Add event for failed playlist file (if needed)
        std::cout << "I forgot to implement the playlist file type (" << (int)Config::PlaylistFileType << ")... please inform me of this, thanks :)";
        return;
    }

    playlistFile->CreatePlaylistFileFromDirectory(_downloadingDir, filePath);
}

void DownloadManager::CleanupThreads()
{
    for (std::thread& thread : _threads) {
        if (thread.joinable())
            thread.join();
    }

    _threads.clear();
}