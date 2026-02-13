#include "IPlatformDownloader.h"

bool IPlatformDownloader::Download(const std::string& url, const std::string& directory)
{
    std::cout << "Downloading..." << std::endl;

    ELinkType linkType = GetLinkType(url);
    if (linkType == ELinkType::Unknown)
        return false;

    bool directoryValid = std::filesystem::exists(directory);
    if (!directoryValid)
        return false;

    switch (linkType) {
        case ELinkType::Track:    return DownloadTrack(url, directory);
        case ELinkType::Playlist: return DownloadPlaylist(url, directory);
        case ELinkType::Album:    return DownloadAlbum(url, directory);
    }
    
    return false;
}

bool IPlatformDownloader::DownloadTrack(const std::string& url, const std::string& directory)
{
    // Get track details
    TrackData track = GetTrack(url);

    // Todo: Move below into seperate files and functions
    //       Just getting it working at the moment

    // Get paths
    std::filesystem::path tempFolder = std::filesystem::temp_directory_path() / APP_NAME;
    if (!std::filesystem::exists(tempFolder))
        std::filesystem::create_directory(tempFolder);

    std::filesystem::path downloadsFolder = tempFolder / DOWNLOADS_FOLDER_NAME;
    if (!std::filesystem::exists(downloadsFolder))
        std::filesystem::create_directory(downloadsFolder);

    std::string fileName = track.Name + " - " + track.Artists[0].Name;
    fileName = FileUtils::ValidateFileName(fileName);

    std::filesystem::path tempDownloadPath = downloadsFolder / fileName;

    std::filesystem::path targetFolder = directory;
    std::filesystem::path targetDownloadPath = targetFolder / fileName;

    // Get cover art
    std::filesystem::path imagesFolder = tempFolder / IMAGES_FOLDER_NAME;
    if (!std::filesystem::exists(imagesFolder))
        std::filesystem::create_directory(imagesFolder);

    std::string imageFileName = track.Album.Name + "(" + track.Artists[0].Name + ")_Cover.png";
    imageFileName = FileUtils::ValidateFileName(imageFileName);

    std::filesystem::path imageFilePath = imagesFolder / imageFileName;

    Image image;
    if (std::filesystem::exists(imageFilePath)) {
        image = ImageHandler::LoadImage(imageFilePath);
    } else {
        image = ImageHandler::DownloadImage(track.Album.ImageUrl);
        ImageHandler::SaveImage(imageFilePath, image);
    }

    // Get the song on the target platform
    std::unique_ptr<IPlatformSearcher> searcher = GetSearcher();
    if (searcher == nullptr) return false;

    PlatformSearcherResult searchResult = searcher->FindTrack(track);

    // Download 
    YtdlpResult downloadResult = Ytdlp::Download(searchResult.Data.Url, tempDownloadPath);
    tempDownloadPath = downloadResult.Path;

    // TODO: Handle errors properly
    if (downloadResult.Error.Error != EYtdlpError::None) {
        std::cout << downloadResult.Error.Details << std::endl;
        return false;
    }

    // Check if downloaded codec is different to the target, if so, convert it
    std::unique_ptr<ICodec> targetCodec = CodecFactory::Create(Config::CODEC_EXTENSION);
    std::unique_ptr<ICodec> downloadedCodec = CodecFactory::Create(tempDownloadPath.extension().string());
    if (targetCodec == nullptr) return false;
    if (downloadedCodec == nullptr) return false;

    if (targetCodec->GetExtension() != downloadedCodec->GetExtension())
        tempDownloadPath = Ffmpeg::Convert(tempDownloadPath, targetCodec->GetExtension());

    // Normalise audio / Set bitrate if manual
    bool normalised = Ffmpeg::Normalise(tempDownloadPath, Config::NORMALISE_DB);

    // Get lyrics

    // Assign metadata

    // Check for errors

    return false;
}

bool IPlatformDownloader::DownloadPlaylist(const std::string& url, const std::string& directory)
{
    PlaylistData playlist = GetPlaylist(url);
    return false;
}   

bool IPlatformDownloader::DownloadAlbum(const std::string& url, const std::string& directory)
{
    AlbumData album = GetAlbum(url);
    return false;
}