#include "Ytdlp.h"

std::string Ytdlp::GetVersion()
{
    Process process = Process::GetRelativeProcess(YTDLP_PATH_RELATIVE);
    process.AddArgument("--version");

    std::string output = process.Execute();
    return output;
}

// Assumes the path has no extension
YtdlpResult Ytdlp::Download(const std::string& url, const std::filesystem::path& pathNoExtension)
{
    YtdlpResult result;

    // Check file path
    std::filesystem::path pathM4a = pathNoExtension.string() + ".m4a";
    std::filesystem::path pathWebm = pathNoExtension.string() + ".webm";

    // Get dependencies paths (move to their own files)
    std::filesystem::path executablePath = FileUtils::GetExecutablePath();
    std::filesystem::path ffmpegPath = executablePath.parent_path() / FFMPEG_PATH_RELATIVE;
    std::filesystem::path quickJsPath = executablePath.parent_path() / QUICKJS_PATH_RELATIVE;

    // Setup new line callback, get details from the download
    unsigned int bitrate = 0;
    unsigned int durationSeconds = 0;
    std::string extension = "";
    std::filesystem::path downloadedPath = pathNoExtension;

    std::string errorString = "";

    std::function<void(std::string)> newLineCallback = [&](std::string line) {
        // Get the download progress
        if (StringUtils::Contains(line, "[download]") && !StringUtils::Contains(line, pathNoExtension.filename())) {
            std::smatch matches;
            if (!std::regex_search(line, matches, std::regex(R"(]\s*(.*)%)")))
                return;

            float progressPercent = std::atof(matches[1].str().c_str()) / 100;

            // Output this to a progress callback
            std::cout << progressPercent << std::endl;
            
            return;
        }

        // Get the details
        if (StringUtils::Contains(line, "DETAILS: ") && !StringUtils::Contains(line, "debug")) {
            std::smatch matches;

            // Bitrate
            if (std::regex_search(line, matches, std::regex(R"(\[bitrate\](\d+))"))) {
                bitrate = std::stoi(matches[1]);
            }

            // Duration
            if (std::regex_search(line, matches, std::regex(R"(\[duration\](.*?)\[)"))) {
                std::string durationString = matches[1];
                durationSeconds = StringUtils::TimeToSeconds(durationString);
            }

            // Extension
            if (std::regex_search(line, matches, std::regex(R"(\[extension\](\w+))"))) {
                extension = matches[1];
                downloadedPath = downloadedPath.string() + "." + extension;
            }
        }

        // Error
        if (StringUtils::Contains(line, "ERROR:"))
            errorString = line;
    };

    // Download
    Process process = Process::GetRelativeProcess(YTDLP_PATH_RELATIVE);
    process.AddArgument("--ffmpeg-location", "\"" + ffmpegPath.string() + "\"");
    process.AddArgument("--js-runtimes", "quickjs:\"" + quickJsPath.string() + "\"");
    process.AddArgument("--print", "\"DETAILS: [bitrate]%(abr)s[duration]%(duration_string)s[extension]%(ext)s\"");
    process.AddArgument("--extractor-args", "\"youtube:player_client=default\"");
    process.AddArgument("-v");
    process.AddArgument("--no-part");
    process.AddArgument("--progress");
    process.AddArgument("--no-simulate");
    process.AddArgument("-f", "ba");
    process.AddArgument("--audio-quality", "0");
    process.AddArgument("-o", "\"" + pathNoExtension.string() + ".%(ext)s\"");
    process.AddArgument("\"" + url + "\"");

    std::string output = process.Execute(newLineCallback);

    YtdlpError error = GetError(errorString);
    if (error.Error != EYtdlpError::None && error.Error != EYtdlpError::LowQuality) {
        result.Error = error;
        return result;
    }

    // Check if the file exists
    if (!std::filesystem::exists(downloadedPath)) {
        error.Details = "Download failed with an unknown error, try downloading again";
        error.Error = EYtdlpError::Unknown;

        result.Error = error;
        return result;;
    }

    result.Path = downloadedPath;
    return result;
}

YtdlpError Ytdlp::GetError(const std::string& errorString)
{
    YtdlpError error;

    if (errorString.empty())
        return error;

    error.Parsed = errorString;
    std::string errorLower = errorString;
    StringUtils::ToLower(errorLower);

    if (StringUtils::Contains(errorLower, "invalid po_token configuration")) {
        error.Details = "PO Token is invalid";
        error.Error = EYtdlpError::InvalidPoToken;
        return error;
    }

    // Check for error 403, means cookies have expired if set, otherwise forbidden
    if (StringUtils::Contains(errorLower, "http error 403: forbidden")) {

        std::cout << "CHECK FOR COOKIES IN YTDLP ERROR" << std::cout;
        //if (cookiesAssigned) {
        //  error.Details = "Your cookies have expired. Please reset them and the PO Token";
        //	error.Error = EYtdlpError::CookiesExpired;
        //  return error;
        //}

        // This shouldnt happen but return just incase
        error.Details = "HTTP Error 403: Forbidden. Please try downloading again or setting cookies";
        error.Error = EYtdlpError::Forbidden;
        return error;
    }

    if (StringUtils::Contains(errorLower, "sign in to confirm youre not a bot")) {
        error.Details = "IP was flagged. Try adding cookies or enabling/disabling a vpn";
        error.Error = EYtdlpError::IpFlagged;
        return error;
    }

    // If video is drm protected, cannot be downloaded
    if (StringUtils::Contains(errorLower, "drm protected") && !StringUtils::Contains(errorLower, "tv client https formats have been skipped as they are drm protected")) {
        error.Details = "Video is DRM protected";
        error.Error = EYtdlpError::DrmProtected;
        return error;
    }

    // If the video does not have a m4a file (majority should)
    if (StringUtils::Contains(errorLower, "requested format is not available")) {
        error.Details = "Video does not have file to download";
        error.Error = EYtdlpError::NoFile;
        return error;
    }

    // Video Unavailable
    if (StringUtils::Contains(errorLower, "video unavailable. this content")) {
        std::cout << "CHECK FOR COOKIES IN YTDLP ERROR" << std::cout;

        std::string errorOutput = "Video is unavailable, try downloading again";
        //if (cookiesAssigned) errorOutput += " or resetting/removing cookies";

        error.Details = errorOutput;
        error.Error = EYtdlpError::Unavailable;
        return error;
    }

    // Check if a low quality version was downloaded with premium
    std::cout << "CHECK FOR PREMIUM IN YTDLP ERROR" << std::endl;
    //if (hasPremium && bitrate < 200) {
    //	qInfo() << SpotifyId << "Does not have a high quality version for the YouTube ID:" << YoutubeId;
    //  error.Details = "Video does not have a high quality version, output will be non-premium quality";
    //  error.Error = EYtdlpError::LowQuality;
    //  return error;
    //}

    error.Parsed = "";
    return error;
}