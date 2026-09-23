#include "TrackDownloader.h"

DownloadResult TrackDownloader::DownloadTrack(const TrackData& track, const EPlatform& searchPlatform, const std::string& directory, std::function<void(DownloadProgress)> progressCallback)
{
    DownloadResult result;
    result.Success = false;

    if (track.Id.empty())
        return result;

    // Should be removed later
    std::cout << "GETTING TRACK: " << track.Name << std::endl;
    track.Print();

    SetProgress(progressCallback, DownloadProgress(0.0, "Setting Up..."));

    // Todo: Move below into seperate files and functions
    //       Just getting it working at the moment

    // == Get paths
    std::filesystem::path tempFolder = std::filesystem::temp_directory_path() / APP_NAME;
    if (!std::filesystem::exists(tempFolder))
        std::filesystem::create_directory(tempFolder);

    std::filesystem::path downloadsFolder = tempFolder / DOWNLOADS_FOLDER_NAME;
    if (!std::filesystem::exists(downloadsFolder))
        std::filesystem::create_directory(downloadsFolder);

    std::unique_ptr<ICodec> targetCodec = CodecFactory::Create(Config::CODEC_EXTENSION);

    std::string fileName = track.Name + " - " + track.Artists[0].Name + "." + targetCodec->GetString();
    fileName = FileUtils::ValidateFileName(fileName);
    
    std::filesystem::path targetFolder = directory;
    std::filesystem::path targetDownloadPath = targetFolder / FileUtils::PathFromUtf8(fileName);
    
    FilePathReserver pathReserver;
    targetDownloadPath = pathReserver.FindAvailableTrackPath(track, targetDownloadPath);

    std::filesystem::path tempDownloadPath = downloadsFolder / targetDownloadPath.stem();

    result.FilePath = targetDownloadPath;

    if (!Config::OVERWRITE && std::filesystem::exists(targetDownloadPath))
        return result;

    // == Get cover art
    // TODO: Make sure it only saves one cover art per album

    std::cout << "Getting Cover Art..." << std::endl;
    SetProgress(progressCallback, DownloadProgress(0.1, "Getting Cover Art..."));

    std::filesystem::path imagesFolder = tempFolder / IMAGES_FOLDER_NAME;
    if (!std::filesystem::exists(imagesFolder))
        std::filesystem::create_directory(imagesFolder);

    std::string imageFileName = track.Album.Name + "(" + track.Artists[0].Name + ")_Cover";
    imageFileName = FileUtils::ValidateFileName(imageFileName);

    std::filesystem::path imageFilePath = imagesFolder / FileUtils::PathFromUtf8(imageFileName);

    Image image;
    if (std::filesystem::exists(imageFilePath)) {
        image = ImageHandler::LoadImage(imageFilePath);
    } else {
        image = ImageHandler::DownloadImage(track.Album.ImageUrl);
        ImageHandler::SaveImage(imageFilePath, image);
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
            return result;
        }
    }

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
        return result;
    }

    // Check if downloaded codec is different to the target, if so, convert it
    std::cout << "Converting..." << std::endl;

    std::unique_ptr<ICodec> downloadedCodec = CodecFactory::Create(tempDownloadPath.extension().string());
    if (targetCodec == nullptr) return result;
    if (downloadedCodec == nullptr) return result;

    if (targetCodec->GetExtension() != downloadedCodec->GetExtension())
        tempDownloadPath = Ffmpeg::Convert(tempDownloadPath, targetCodec->GetExtension());

    // == Normalise
    float progressStartPercentage = 0.7;
    float progressEndPercentage = Config::GET_LYRICS ? 0.9 : 1.0;

    if (Config::NORMALISE) {
        std::cout << "Normalising..." << std::endl;
        SetProgress(progressCallback, DownloadProgress(progressStartPercentage, "Normalising Audio..."));

        bool normalised = Ffmpeg::Normalise(tempDownloadPath, Config::NORMALISE_DB, [&](float progress) {
            SetProgress(progressCallback, DownloadProgress(MathUtils::Lerp(progressStartPercentage, progressEndPercentage, progress), "Normalising Audio..."));
        });
    }
    
    // == Set bitrate
    else if (Config::MANUAL_BITRATE) {
        std::cout << "Setting bitrate..." << std::endl;
        SetProgress(progressCallback, DownloadProgress(progressStartPercentage, "Setting Bitrate..."));

        bool bitrateSet = Ffmpeg::SetBitrate(tempDownloadPath, Config::BITRATE, [&](float progress) {
            SetProgress(progressCallback, DownloadProgress(MathUtils::Lerp(progressStartPercentage, progressEndPercentage, progress), "Setting Bitrate..."));
        });
    }

    // == Get lyrics
    if (Config::GET_LYRICS) {
        std::cout << "Getting Lyrics..." << std::endl;
        SetProgress(progressCallback, DownloadProgress(progressEndPercentage, "Getting Lyrics..."));

        // Try source platform
        Lyrics lyrics = LyricsFinder::GetSourceLyrics(track);
        
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
    if (Config::OVERWRITE && std::filesystem::exists(targetDownloadPath))
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
        if (!copied) return result;
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

int TrackDownloader::DownloadTracks(const std::vector<TrackData>& tracks, const EPlatform& searchPlatform, const std::string& directory, std::function<void(int, DownloadProgress)> progressCallback, std::function<void(int, DownloadResult)> trackDownloadedCallback)
{
    int downloadErrors = 0;
    for (int i = 0; i < tracks.size(); i++) {
        const TrackData& track = tracks[i];

        std::function<void(DownloadProgress)> callback = nullptr;
        if (progressCallback != nullptr)
            callback = [&](DownloadProgress p) { progressCallback(i, p); };

        DownloadResult downloadResult = DownloadTrack(track, searchPlatform, directory, callback);
        if (trackDownloadedCallback != nullptr)
            trackDownloadedCallback(i, downloadResult);

        if (!downloadResult.Success) downloadErrors++;
    }

    return downloadErrors;
}