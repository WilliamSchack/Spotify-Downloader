#include "DownloadManager.h"

bool DownloadManager::Download(const std::string& url, const std::string& directory)
{
    bool directoryValid = std::filesystem::exists(directory);
    if (!directoryValid)
        return false;

    EPlatform platformType = PlatformDetector::GetPlatformFromUrl(url);
    if (platformType == EPlatform::Unknown) return false;

    std::unique_ptr<IPlatformDownloader> platform = PlatformFactory::CreateDownloader(platformType);
    if (platform == nullptr) return false;

    EPlatform searchPlatform = platform->GetSearchPlatform();
    if (searchPlatform == EPlatform::Unknown) return false;

    // Get the tracks
    ELinkType linkType = platform->GetLinkType(url);
    if (linkType == ELinkType::Unknown)
        return false;

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
            return false;
    }

    if (tracks.size() == 0)
        return false;

    // Get track distribution
    int songCount = tracks.size();
    int threadCount = std::min(songCount, Config::PER_DOWNLOAD_THREADS);

    int baseSongCount = songCount / threadCount;
    int songsRemainder = songCount % threadCount;

    // Dispatch threads
    int currentStartIndex = 0;
    for (int i = 0; i < threadCount; i++) {
        int currentSongCount = songsRemainder == 0 ? baseSongCount : baseSongCount + 1;
        if (songsRemainder > 0) songsRemainder--;

        std::vector<TrackData> threadTracks(tracks.begin() + currentStartIndex, tracks.begin() + currentStartIndex + currentSongCount);
        
        std::thread thread([threadTracks, searchPlatform, directory]() {
            DownloadManager::ThreadDownload(threadTracks, searchPlatform, directory);
        });

        thread.detach();

        currentStartIndex += currentSongCount;
    }

    // Have a callback when the threads are finished
    // Could wait here but it would block the main thread

    return true;
}

void DownloadManager::ThreadDownload(const std::vector<TrackData>& tracks, const EPlatform& searchPlatform, const std::string& directory)
{
    std::cout << "THREAD: " << std::this_thread::get_id() << std::endl;

    TrackDownloader::DownloadTracks(tracks, searchPlatform, directory);

    std::cout << "THREAD: " << std::this_thread::get_id() << " FINISHED DOWNLOADING" << std::endl;

    // Figure out thread redistribution
}