#include "TrackDownloader.h"

DownloadResult TrackDownloader::DownloadTrack(const TrackData& track, const EPlatform& searchPlatform, const std::string& directory, std::function<void(DownloadProgress)> progressCallback, std::function<void(std::filesystem::path)> coverArtDownloadedCallback)
{
    DownloadResult result;
    result.Success = false;

    if (track.Platform == EPlatform::Unknown || track.Id.empty())
        return result;

    // Should be removed later
    std::cout << "GETTING TRACK: " << track.Name << std::endl;
    track.Print();

    SetProgress(progressCallback, DownloadProgress(0.0, "Setting Up..."));

    // Todo: Move below into seperate files and functions
    //       Just getting it working at the moment

    // == Get paths
    std::unique_ptr<ICodec> targetCodec = CodecFactory::Create(Config::CodecExtension);
    if (targetCodec == nullptr) {
        result.FailReason = "Could not create the codec (" + std::to_string((int)Config::CodecExtension) + "), please inform me of this error";
        return result;
    }

    std::string fileName = track.Name + " - " + track.Artists[0].Name + "." + targetCodec->GetString();
    fileName = FileUtils::ValidateFileName(fileName);
    
    std::filesystem::path targetFolder = directory;
    std::filesystem::path targetDownloadPath = targetFolder / FileUtils::PathFromUtf8(fileName);
    
    FilePathReserver pathReserver;
    targetDownloadPath = pathReserver.FindAvailableTrackPath(track, targetDownloadPath);

    std::filesystem::path tempDownloadPath = TemporaryPaths::GetTrackDownloadPathNoExtension(track);

    result.FilePath = targetDownloadPath;

    if (!Config::Overwrite && std::filesystem::exists(targetDownloadPath)) {
        std::cout << "Not downloading as it already exists: " << track.Name << std::endl;

        // Showing as a success since its already downloaded
        result.Success = true;
        return result;
    }

    // == Get cover art
    std::cout << "Getting Cover Art..." << std::endl;
    SetProgress(progressCallback, DownloadProgress(0.1, "Getting Cover Art..."));

    std::filesystem::path imageFilePath = TemporaryPaths::GetTrackImagePathNoExtension(track);
    Image image;

    {
        // In the case that multiple threads try to download the same cover art at the same time, the image will corrupt
        // This prevents that
        // TODO: Cleanup these mutexes as they are only deleted when the app closes
        std::lock_guard<std::mutex> getCoverArtLock(GetCoverArtMutex(track.Album.GetUniqueId()));
        
        std::filesystem::path existingImageFilePath = FileUtils::FindPathWithAnyExtension(imageFilePath.parent_path(), imageFilePath.filename());

        if (!existingImageFilePath.empty()) {
            image = ImageHandler::LoadImage(existingImageFilePath);
        } else {
            image = ImageHandler::DownloadImage(track.Album.ImageUrl);
            std::filesystem::path imagePath = ImageHandler::SaveImage(imageFilePath, image);

            if (!imagePath.empty() && coverArtDownloadedCallback != nullptr)
                coverArtDownloadedCallback(imagePath);
        }
    }

    // == Get the song on the target platform
    SetProgress(progressCallback, DownloadProgress(0.1, "Searching..."));

    PlatformSearcherResult searchResult;
    
    if (track.Platform == searchPlatform) {
        // No need to search if its on the same platform
        searchResult.Confidence = 1;
        searchResult.Data = track;
    } else {
        std::cout << "Searching on different platform..." << std::endl;

        std::unique_ptr<IPlatformSearcher> searcher = PlatformFactory::CreateSearcher(searchPlatform);
        if (searcher == nullptr) return result;

        searchResult = searcher->FindTrack(track, [&](float progress) {
            SetProgress(progressCallback, DownloadProgress(MathUtils::Lerp(0.1, 0.3, progress), "Searching..."));
        });

        if (searchResult.Data.Platform == EPlatform::Unknown) {
            std::cout << "Could not find track: " << track.Name << std::endl;

            // TODO: let the user input a platform url to download in this case

            // TODO: Insert which search platform
            result.FailReason = "Track cannot be found on the search platform";
            return result;
        }
    }

    // TODO: Check for youtube flagging your ip

    // == Download 
    std::cout << "Downloading..." << std::endl;
    SetProgress(progressCallback, DownloadProgress(0.3, "Downloading Track..."));

    YtdlpResult downloadResult = Ytdlp::Download(searchResult.Data.Url, tempDownloadPath, [&](float progress) {
        SetProgress(progressCallback, DownloadProgress(MathUtils::Lerp(0.3, 0.7, progress), "Downloading Track..."));
    });

    tempDownloadPath = downloadResult.Path;

    // TODO: Handle errors properly
    if (downloadResult.Error.Error != EYtdlpError::None) {
        std::cout << downloadResult.Error.Details << std::endl;
        result.FailReason = downloadResult.Error.Details;
        return result;
    }

    // Check if downloaded codec is different to the target, if so, convert it
    std::cout << "Converting..." << std::endl;

    std::unique_ptr<ICodec> downloadedCodec = CodecFactory::Create(tempDownloadPath.extension().string());
    if (downloadedCodec == nullptr) {
        result.FailReason = "Could not create the codec (" + std::to_string((int)Config::CodecExtension) + "), please inform me of this error";
        return result;
    }

    if (targetCodec->GetExtension() != downloadedCodec->GetExtension())
        tempDownloadPath = Ffmpeg::Convert(tempDownloadPath, targetCodec->GetExtension());

    // == Normalise
    float progressStartPercentage = 0.7;
    float progressEndPercentage = Config::GetLyrics ? 0.9 : 1.0;

    if (Config::Normalise) {
        std::cout << "Normalising..." << std::endl;
        SetProgress(progressCallback, DownloadProgress(progressStartPercentage, "Normalising Audio..."));

        bool normalised = Ffmpeg::Normalise(tempDownloadPath, Config::NormaliseDb, [&](float progress) {
            SetProgress(progressCallback, DownloadProgress(MathUtils::Lerp(progressStartPercentage, progressEndPercentage, progress), "Normalising Audio..."));
        });
    }
    
    // == Set bitrate
    else if (Config::ManualBitrate) {
        std::cout << "Setting bitrate..." << std::endl;
        SetProgress(progressCallback, DownloadProgress(progressStartPercentage, "Setting Bitrate..."));

        bool bitrateSet = Ffmpeg::SetBitrate(tempDownloadPath, Config::BitrateKbps, [&](float progress) {
            SetProgress(progressCallback, DownloadProgress(MathUtils::Lerp(progressStartPercentage, progressEndPercentage, progress), "Setting Bitrate..."));
        });
    }

    // == Get lyrics
    Lyrics lyrics;
    if (Config::GetLyrics) {
        std::cout << "Getting Lyrics..." << std::endl;
        SetProgress(progressCallback, DownloadProgress(progressEndPercentage, "Getting Lyrics..."));

        // Try source platform
        lyrics = LyricsFinder::GetSourceLyrics(track);
        
        // Try searched platform
        if (lyrics.Type == ELyricsType::None)
            lyrics = LyricsFinder::GetSourceLyrics(searchResult.Data);

        // Try external platforms
        if (lyrics.Type == ELyricsType::None)
            lyrics = LyricsFinder::GetBestLyrics(track);

        // TODO: Create LRC File
    }

    // == Assign metadata
    SetProgress(progressCallback, DownloadProgress(1.0, "Assigning Metadata..."));

    std::string publisherText = "Downloaded through " + std::string(APP_NAME) + " by William Schack";
    std::string copyrightText = "";
    copyrightText += "Source: " + PlatformUtils::GetPlatformString(track.Platform) + " (" + track.Id + ")";
    copyrightText += ", Downloaded: " + PlatformUtils::GetPlatformString(searchResult.Data.Platform) + " (" + searchResult.Data.Id + ")";
    if (lyrics.Type != ELyricsType::None) copyrightText += ", Lyrics: (" + lyrics.SourceMessage + ")";
    std::string commentText = "Thanks for using my program! :) - William S";

    MetadataManager metadata(tempDownloadPath);
    metadata.SetCoverImage(image);
    metadata.SetTitle(track.Name);
    metadata.SetArtists(track.Artists);
    metadata.SetAlbumName(track.Album.Name);
    metadata.SetAlbumArtists(track.Album.Artists);
    metadata.SetPublisher(publisherText);
    metadata.SetCopyright(copyrightText);
    metadata.SetComment(commentText);
    metadata.SetReleaseDate(track.ReleaseDate);
    metadata.SetTrackNumber(track.TrackNumber); // Use playlist number if a playlist (might actually store playlist number in the regular track number from DownloadManager if a playlist)
    metadata.SetDiscNumber(track.DiscNumber);
    if (lyrics.Type != ELyricsType::None) metadata.SetLyrics(lyrics.GetString());
    metadata.Close();

    // == Move to target path
    if (Config::Overwrite && std::filesystem::exists(targetDownloadPath))
        std::filesystem::remove(targetDownloadPath);
    
    if (!std::filesystem::is_directory(targetFolder))
        std::filesystem::create_directory(targetFolder);

    try {
        std::filesystem::rename(tempDownloadPath, targetDownloadPath);
    } catch (...) {
        // If rename fails, assume it is between drives, in that case copy
        bool copied = std::filesystem::copy_file(tempDownloadPath, targetDownloadPath);
        std::filesystem::remove(tempDownloadPath);

        // Move error
        if (!copied) {
            result.FailReason = "Could not move the file from the temporary folder";
            return result;
        }
    }

    result.Success = true;
    return result;
}

void TrackDownloader::SetProgress(const std::function<void(DownloadProgress)>& progressCallback, const DownloadProgress& progress)
{
    if (progressCallback == nullptr)
        return;

    progressCallback(progress);
}

std::mutex& TrackDownloader::GetCoverArtMutex(const std::string& albumUniqueId)
{
    std::lock_guard<std::mutex> lock(_coverArtMapMutex);
    return _coverArtMutexes[albumUniqueId];
}

int TrackDownloader::DownloadTracks(const std::vector<TrackData>& tracks, const EPlatform& searchPlatform, const std::string& directory, std::function<void(int, DownloadProgress)> progressCallback, std::function<void(int, std::filesystem::path)> coverArtDownloadedCallback, std::function<void(int, DownloadResult)> trackDownloadedCallback)
{
    int downloadErrors = 0;
    for (int i = 0; i < tracks.size(); i++) {
        const TrackData& track = tracks[i];

        std::function<void(DownloadProgress)> progressCallbackIndividual = nullptr;
        if (progressCallback != nullptr)
            progressCallbackIndividual = [&](DownloadProgress p) { progressCallback(i, p); };

        std::function<void(std::filesystem::path)> coverArtDownloadedCallbackIndividual = nullptr;
        if (coverArtDownloadedCallback != nullptr)
            coverArtDownloadedCallbackIndividual = [&](std::filesystem::path p) { coverArtDownloadedCallback(i, p); };

        DownloadResult downloadResult = DownloadTrack(track, searchPlatform, directory, progressCallbackIndividual, coverArtDownloadedCallbackIndividual);
        if (trackDownloadedCallback != nullptr)
            trackDownloadedCallback(i, downloadResult);

        if (!downloadResult.Success) downloadErrors++;
    }

    return downloadErrors;
}