#include "ArgumentParser.h"

// Returns if the args were valid
bool ArgumentParser::ParseBeforeGUI(int argc, char** argv)
{
    if (argc <= 1)
        return false;

    std::string command = argv[1];

    if (command == "download-deps" && argc == 2) {
        Logger::Info("Downloading ffmpeg...");
        if (!Ffmpeg::DownloadExecutable()) {
            if (Ffmpeg::IsManaged())
                Logger::Info("ffmpeg is already downloaded");
            else
                Logger::Warning("Failed to download ffmpeg");
        } else {
            Logger::Info("Successfully downloaded ffmpeg");
        }

        Logger::Info("Downloading yt-dlp...");
        if (!Ytdlp::DownloadExecutable()) {
            if (Ytdlp::IsManaged())
                Logger::Info("yt-dlp is already downloaded");
            else
                Logger::Warning("Failed to download yt-dlp");
        } else {
            Logger::Info("Successfully downloaded yt-dlp");
        }

        Logger::Info("Downloading quickjs...");
        if (!JsRuntime::DownloadExecutable()) {
            if (JsRuntime::IsManaged())
                Logger::Info("quickjs is already downloaded");
            else
                Logger::Warning("Failed to download quickjs");
        } else {
            Logger::Info("Successfully downloaded quickjs");
        }

        return true;
    }
    return false;
}

// Requires QtWebEngine to be setup
bool ArgumentParser::ParseAfterGUI(int argc, char** argv)
{
    if (argc <= 1)
        return false;

    std::string command = argv[1];

    if (command == "download-track" && argc == 4) {
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
            Logger::Out("Successfully downloaded (" + FileUtils::PathToUtf8(downloadedResult.FilePath) + ")");
            // Will output download results for each track with file names etc. when properly implementing the cli
        } else {
            Logger::Warning("Failed to download");
        }

        return true;
    }

    if (command == "get-info" && argc == 3) {
        std::string url = argv[2];
        EPlatform platformType = PlatformDetector::GetPlatformFromUrl(url);
        if (platformType == EPlatform::Unknown) return true;

        std::unique_ptr<IPlatformDownloader> platform = PlatformFactory::CreateDownloader(platformType);
        ELinkType linkType = platform->GetLinkType(url);
        if (linkType == ELinkType::Unknown) return true;
        
        switch (linkType) {
            case ELinkType::Track:
                Logger::Out(platform->GetTrack(url).ToJson().dump(4));
                break;
            case ELinkType::Playlist:
                Logger::Out(platform->GetPlaylist(url).ToJson().dump(4));
                break;
            case ELinkType::Album:
                Logger::Out(platform->GetAlbum(url).ToJson().dump(4));
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
        Logger::Out(searchResult.ToJson().dump(4));
        
        return true;
    }

    if (command == "get-metadata" && argc == 3) {
        std::string filePath = argv[2];

        MetadataManager metadata(filePath, true);
        TrackData trackData = metadata.GetAll();

        Logger::Out(trackData.ToJson().dump(4));

        return true;
    }

    return false;
}