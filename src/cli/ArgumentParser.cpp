#include "ArgumentParser.h"

// Returns if the args were valid
bool ArgumentParser::Parse(int argc, char** argv)
{
    if (argc <= 1)
        return false;

    std::string command = argv[1];

    if (command == "downloadtrack" && argc == 4) {
        std::string url = argv[2];
        std::string downloadFolder = argv[3];

        // Would use download manager but I cant be fucked dealing with threads atm
        // I will implement non-tracks when I properly implement the cli

        EPlatform platformType = PlatformDetector::GetPlatformFromUrl(url);
        if (platformType == EPlatform::Unknown) return true;

        std::unique_ptr<IPlatformDownloader> platform = PlatformFactory::CreateDownloader(platformType);

        ELinkType linkType = platform->GetLinkType(url);
        if (linkType != ELinkType::Track) return true;

        EPlatform searchPlatform = platform->GetSearchPlatform();
        if (searchPlatform == EPlatform::Unknown) return true;

        TrackData track = platform->GetTrack(url);
        bool downloaded = TrackDownloader::DownloadTrack(track, searchPlatform, downloadFolder);

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