#include "ArgumentParser.h"

// Returns if the args were valid
bool ArgumentParser::Parse(const int& argc, const char** argv)
{
    if (argc <= 1)
        return false;

    std::string command = argv[1];

    if (command == "download" && argc == 4) {
        std::string url = argv[2];
        std::string downloadFolder = argv[3];
        bool downloaded = DownloadManager::Download(url, downloadFolder);

        if (downloaded) {
            std::cout << "Successfully downloaded" << std::endl;
            // Will output download results for each track with file names etc. when properly implementing the cli
        } else {
            std::cout << "Failed to download" << std::endl;
        }

        return true;
    }

    if (command == "trackinfo" && argc == 3) {
        std::string url = argv[2];
        EPlatform platformType = PlatformDetector::GetPlatformFromUrl(url);
        if (platformType == EPlatform::Unknown) return true;

        std::unique_ptr<IPlatformDownloader> platform = PlatformFactory::CreateDownloader(platformType);
        ELinkType linkType = platform->GetLinkType(url);
        if (linkType != ELinkType::Track) return false;
        
        TrackData track = platform->GetTrack(url);
        track.Print();

        return true;
    }

    return false;
}