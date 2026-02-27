#include "DownloadManager.h"

bool DownloadManager::Download(const std::string& url, const std::string& directory)
{
    bool directoryValid = std::filesystem::exists(directory);
    if (!directoryValid)
        return false;

    EPlatform platformType = PlatformDetector::GetPlatformFromUrl(url);
    if (platformType == EPlatform::Unknown) return false;

    // Should change this to have another middleman instead of using IPlatformDownloader and creating more later
    std::unique_ptr<IPlatformDownloader> platform = PlatformFactory::Create(platformType);
    if (platform == nullptr) return false;

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
    }

    if (tracks.size() == 0)
        return false;

    // TODO: Add validation checks here (Fix multiple of same file name)
    // https://github.com/WilliamSchack/Spotify-Downloader/blob/d0789b4713bde0751e75642e9cf373176750a522/Spotify%20Downloader/Downloading/PlaylistDownloader.cpp#L109

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
        
        std::thread thread([threadTracks, platformType, directory]() {
            DownloadManager::ThreadDownload(threadTracks, platformType, directory);
        });

        thread.detach();

        currentStartIndex += currentSongCount;
    }

    // Shouldnt return anything, instead have a callback when the threads are finished
    // Could wait here but it would block the main thread

    return true;
}

void DownloadManager::ThreadDownload(const std::vector<TrackData>& tracks, const EPlatform& platformType, const std::string& directory)
{
    std::cout << "THREAD: " << std::this_thread::get_id() << std::endl;

    std::unique_ptr<IPlatformDownloader> platform = PlatformFactory::Create(platformType);
    platform->DownloadTracks(tracks, directory);

    std::cout << "THREAD: " << std::this_thread::get_id() << " FINISHED DOWNLOADING" << std::endl;

    // Figure out thread redistribution
}