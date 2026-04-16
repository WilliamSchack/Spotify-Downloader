#include "TrackDownloader.h"

DownloadResult TrackDownloader::DownloadTrack(const TrackData& track, const EPlatform& searchPlatform, const std::string& directory)
{
    DownloadResult result;
    result.Success = false;

    if (track.Id.empty())
        return result;

    std::cout << "GETTING TRACK: " << track.Name << std::endl;
    track.Print();

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

#ifdef WIN32
    std::wstring validatedTrackName = FileUtils::ValidateFileName(StringUtils::ToWString(track.Name));
    std::wstring validatedArtistName = FileUtils::ValidateFileName(StringUtils::ToWString(track.Artists[0].Name));
    std::wstring codecString = StringUtils::ToWString(targetCodec->GetString());
    
    std::wstring fileName = validatedTrackName + L" - " + validatedArtistName + L"." + codecString;
#else
    std::string validatedTrackName = FileUtils::ValidateFileName(track.Name);
    std::string validatedArtistName = FileUtils::ValidateFileName(track.Artists[0].Name);
    std::string codecString = targetCodec->GetString();
    
    std::string fileName = validatedTrackName + " - " + validatedArtistName + "." + codecString;
#endif
    
    std::filesystem::path targetFolder = directory;
    std::filesystem::path targetDownloadPath = targetFolder / fileName;
    
    FilePathReserver pathReserver;
    targetDownloadPath = pathReserver.FindAvailableTrackPath(track, targetDownloadPath);

    std::filesystem::path tempDownloadPath = downloadsFolder / targetDownloadPath.stem();

    result.FilePath = targetDownloadPath;

    if (!Config::OVERWRITE && std::filesystem::exists(targetDownloadPath))
        return result;

    // == Get cover art
    // TODO: Make sure it only saves one cover art per album

    std::cout << "Getting cover art..." << std::endl;

    std::filesystem::path imagesFolder = tempFolder / IMAGES_FOLDER_NAME;
    if (!std::filesystem::exists(imagesFolder))
        std::filesystem::create_directory(imagesFolder);

    std::wstring imageFileName = StringUtils::ToWString(track.Album.Name) + L"(" + StringUtils::ToWString(track.Artists[0].Name) + L")_Cover";
    imageFileName = FileUtils::ValidateFileName(imageFileName);

    std::filesystem::path imageFilePath = imagesFolder / imageFileName;

    Image image;
    if (std::filesystem::exists(imageFilePath)) {
        image = ImageHandler::LoadImage(imageFilePath);
    } else {
        image = ImageHandler::DownloadImage(track.Album.ImageUrl);
        ImageHandler::SaveImage(imageFilePath, image);
    }

    // == Get the song on the target platform
    PlatformSearcherResult searchResult;
    
    if (track.Platform == searchPlatform) {
        // No need to search if its on the same platform
        searchResult.Confidence = 1;
        searchResult.Data = track;
    } else {
        std::cout << "Searching on different platform..." << std::endl;

        std::unique_ptr<IPlatformSearcher> searcher = PlatformFactory::CreateSearcher(searchPlatform);
        if (searcher == nullptr) return result;

        searchResult = searcher->FindTrack(track);
        if (searchResult.Data.Platform == EPlatform::Unknown) {
            std::cout << "Could not find track: " << track.Name << std::endl;
            return result;
        }
    }

    // == Download 
    std::cout << "Downloading..." << std::endl;

    YtdlpResult downloadResult = Ytdlp::Download(searchResult.Data.Url, tempDownloadPath);
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
    if (Config::NORMALISE) {
        std::cout << "Normalising..." << std::endl;
        bool normalised = Ffmpeg::Normalise(tempDownloadPath, Config::NORMALISE_DB);
    }
    
    // == Set bitrate
    if (Config::MANUAL_BITRATE) {
        std::cout << "Setting bitrate..." << std::endl;
        bool bitrateSet = Ffmpeg::SetBitrate(tempDownloadPath, Config::BITRATE);
    }

    // == Get lyrics
    std::cout << "Getting Lyrics..." << std::endl;

    // Try source platform
    Lyrics lyrics = LyricsFinder::GetSourceLyrics(track);
    
    // Try searched platform
    if (lyrics.Type == ELyricsType::None)
        lyrics = LyricsFinder::GetSourceLyrics(searchResult.Data);

    // Try external platforms
    if (lyrics.Type == ELyricsType::None)
        lyrics = LyricsFinder::GetBestLyrics(track);

    // == Assign metadata
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

int TrackDownloader::DownloadTracks(const std::vector<TrackData>& tracks, const EPlatform& searchPlatform, const std::string& directory)
{
    int downloadErrors = 0;
    for (const TrackData& track : tracks) {
        DownloadResult downloadResult = DownloadTrack(track, searchPlatform, directory);
        if (!downloadResult.Success) downloadErrors++;
    }

    return downloadErrors;
}