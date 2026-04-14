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
        DownloadResult downloadedResult = TrackDownloader::DownloadTrack(track, searchPlatform, downloadFolder);

        if (downloadedResult.Success) {
            std::cout << "Successfully downloaded (" << downloadedResult.FilePath << ")" << std::endl;
            // Will output download results for each track with file names etc. when properly implementing the cli
        } else {
            std::cout << "Failed to download" << std::endl;
        }

        return true;
    }

    if (command == "getinfo" && argc == 3) {
        std::string url = argv[2];
        EPlatform platformType = PlatformDetector::GetPlatformFromUrl(url);
        if (platformType == EPlatform::Unknown) return true;

        std::unique_ptr<IPlatformDownloader> platform = PlatformFactory::CreateDownloader(platformType);
        ELinkType linkType = platform->GetLinkType(url);
        if (linkType == ELinkType::Unknown) return true;
        
        switch (linkType) {
            case ELinkType::Track:
                platform->GetTrack(url).Print();
                break;
            case ELinkType::Playlist:
                platform->GetPlaylist(url).Print();
                break;
            case ELinkType::Album:
                platform->GetAlbum(url).Print();
                break;
        }

        return true;
    }

    if (command == "search" && argc == 3) {
        std::string url = argv[2];
        EPlatform platformType = PlatformDetector::GetPlatformFromUrl(url);
        if (platformType == EPlatform::Unknown) return true;

        std::unique_ptr<IPlatformDownloader> platform = PlatformFactory::CreateDownloader(platformType);
        ELinkType linkType = platform->GetLinkType(url);
        if (linkType != ELinkType::Track) return true;

        EPlatform searchPlatform = platform->GetSearchPlatform();
        if (searchPlatform == EPlatform::Unknown) return true;

        std::unique_ptr<IPlatformSearcher> searcher = PlatformFactory::CreateSearcher(searchPlatform);
        if (searcher == nullptr) return true;

        TrackData track = platform->GetTrack(url);
        if (track.Platform == EPlatform::Unknown) return true;

        PlatformSearcherResult searchResult = searcher->FindTrack(track);
        searchResult.Print();
        
        return true;
    }

    if (command == "getmetadata" && argc == 3) {
        std::string filePath = argv[2];

        MetadataManager metadata(filePath);
        TrackData trackData = metadata.GetAll();

        trackData.Print();

        return true;
    }

    return false;
}