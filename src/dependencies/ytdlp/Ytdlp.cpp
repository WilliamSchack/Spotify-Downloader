#include "Ytdlp.h"

bool Ytdlp::DownloadExecutableImpl(std::filesystem::path path)
{
    // Get the download details
    std::string url = "https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp";

#ifdef WIN32
    #ifdef IS_ARM_CPU
        url += "_arm64.exe";
    #else
        url += ".exe";
    #endif
#elif __APPLE__
    url += "_macos";
#else
    #ifdef IS_ARM_CPU
        url += "_aarch64_linux";
    #else
        url += "_linux";
    #endif
#endif

    NetworkRequest request;
    request.Url = url;
    request.DownloadToFile(path);

    if (!std::filesystem::exists(path))
        return false;

#ifndef WIN32
    // Make it executable
    std::filesystem::permissions(
        path,
        std::filesystem::perms::owner_exec | std::filesystem::perms::group_exec | std::filesystem::perms::others_exec,
        std::filesystem::perm_options::add
    );
#endif

    return true;
}

bool Ytdlp::UpdateExecutableImpl()
{
    std::filesystem::path path = GetPath();
    if (path.empty())
        return false;

    ExternalProcess process(path);
    process.AddArgument("-U");

    std::string output = process.Execute().Output;
    if (StringUtils::StartsWith(output, "ERROR:")) {
        std::cout << "Failed to update yt-dlp with the output: " << output << std::endl;
        return false;
    }

    return true;
}

std::string Ytdlp::GetVersionImpl()
{
    std::filesystem::path path = GetPath();
    if (path.empty())
        return "";

    ExternalProcess process(path);
    process.AddArgument("--version");

    std::string output = process.Execute().Output;
    output = StringUtils::RemoveChar(output, ' ');
    output = StringUtils::RemoveChar(output, '\n');

    return output;
}

std::string Ytdlp::GetLatestVersionImpl()
{
    return GitHubAPI::GetLatestTag("yt-dlp", "yt-dlp");
}

std::string Ytdlp::GetFileNameImpl()
{
    return "yt-dlp";
}

std::vector<GitHubRelease> Ytdlp::GetReleasesImpl(const int& resultsPerPage, const int& page)
{
    return GitHubAPI::GetReleases("yt-dlp", "yt-dlp", resultsPerPage, page);
}

// Assumes the path has no extension
YtdlpResult Ytdlp::Download(const std::string& url, const std::filesystem::path& pathNoExtension, const std::vector<std::string>& extraArgs, std::function<void(float)> progressCallback)
{
    YtdlpResult result;

    std::filesystem::path ytdlpPath = GetPath();
    std::filesystem::path ffmpegPath = Ffmpeg::GetPath();
    std::filesystem::path quickJsPath = JsRuntime::GetPath();
    if (ytdlpPath.empty() || ffmpegPath.empty() || quickJsPath.empty()) {
        std::cout << "yt-dlp and/or its dependencies are not installed: ";
        std::cout << "yt-dlp (" << (ytdlpPath.empty() ? "NOT INSTALLED" : "INSTALLED") << ") ";
        std::cout << "ffmpeg (" << (ffmpegPath.empty() ? "NOT INSTALLED" : "INSTALLED") << ") ";
        std::cout << "quickjs (" << (quickJsPath.empty() ? "NOT INSTALLED" : "INSTALLED") << ") ";
        std::cout << std::endl;

        YtdlpError error;
        error.Details = "yt-dlp is not installed";
        error.Error = EYtdlpError::NotInstalled;
        result.Error = error;

        return result;
    }

    if (url.empty()) {
        std::cout << "Cannot download, no url was passed" << std::endl;
        return result;
    }

    // Remove leftover download if it exists
    std::filesystem::path pathM4a = pathNoExtension;
    std::filesystem::path pathWebm = pathNoExtension;
    pathM4a += ".m4a";
    pathWebm += ".webm";

    if (std::filesystem::exists(pathM4a))
        std::filesystem::remove(pathM4a);
    if (std::filesystem::exists(pathWebm))
        std::filesystem::remove(pathWebm);

    // Setup new line callback, get details from the download
    unsigned int bitrate = 0;
    unsigned int durationSeconds = 0;
    std::string extension = "";
    std::filesystem::path downloadedPath = pathNoExtension;

    std::string errorString = "";

    std::function<void(std::string)> newLineCallback = [&](std::string line) {
        // Get the download progress
        if (progressCallback != nullptr &&
            StringUtils::Contains(line, "[download]") &&
            !StringUtils::Contains(line, FileUtils::PathToUtf8(pathNoExtension.filename()))
            ) {

            std::smatch matches;
            if (!std::regex_search(line, matches, std::regex(R"(\]\s*([\d.]+)%)")))
                return;

            float progressPercent = std::atof(matches[1].str().c_str()) / 100;
            progressCallback(progressPercent);
            
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
                downloadedPath += "." + extension;
            }
        }

        // Error
        if (StringUtils::Contains(line, "ERROR:"))
            errorString = line;
    };

    // Download
    ExternalProcess process(ytdlpPath);
    process.AddArgument("--ffmpeg-location", ffmpegPath.string());
    process.AddArgument("--js-runtimes", "quickjs:" + quickJsPath.string());
    process.AddArgument("--print", "DETAILS: [bitrate]%(abr)s[duration]%(duration_string)s[extension]%(ext)s");
    process.AddArgument("-v");
    process.AddArgument("--no-part");
    process.AddArgument("--progress");
    process.AddArgument("--no-simulate");
    process.AddArgument("-f", "ba");
    process.AddArgument("--audio-quality", "0");

    if (!MathUtils::DoublesEqual(0, Config::DownloadSpeedLimit))
        process.AddArgument("--limit-rate", std::to_string(Config::DownloadSpeedLimit) + "M");

    switch (Config::IPVersion) {
        case EIPVersion::IPv4:
            process.AddArgument("--force-ipv4");
            break;
        case EIPVersion::IPv6:
            process.AddArgument("--force-ipv6");
            break;
    }

    for (std::string arg : extraArgs) {
        process.AddArgument(arg);
    }

    process.AddArgument("-o", FileUtils::PathToUtf8(pathNoExtension) + ".%(ext)s");
    process.AddArgument(url);

    ExternalProcessResult outputResult = process.Execute(Config::DownloadTimeoutMs, newLineCallback);
    if (outputResult.TimedOut) {
        YtdlpError error;
        error.Details = "Download timed out";
        error.Error = EYtdlpError::TimedOut;
        result.Error = error;
        return result;
    }

    std::string output = outputResult.Output;

    YtdlpError error = GetError(errorString);
    if (error.Error != EYtdlpError::None && error.Error != EYtdlpError::LowQuality) {
        std::cout << "yt-dlp Error Output: " << output << std::endl;
        result.Error = error;
        return result;
    }

    // Check if the file exists
    if (!std::filesystem::exists(downloadedPath)) {
        std::cout << output << std::endl;

        error.Details = "Download failed with an unknown error, try downloading again";
        error.Error = EYtdlpError::Unknown;

        result.Error = error;
        return result;
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
    std::string errorLower = StringUtils::ToLower(errorString);
    

    if (StringUtils::Contains(errorLower, "invalid po_token configuration")) {
        error.Details = "PO Token is invalid";
        error.Error = EYtdlpError::InvalidPoToken;
        return error;
    }

    // Check for error 403, means cookies have expired if set, otherwise forbidden
    if (StringUtils::Contains(errorLower, "http error 403: forbidden")) {

        std::cout << "CHECK FOR COOKIES IN YTDLP ERROR: " << errorString << std::endl;
        //if (cookiesAssigned) {
        //  error.Details = "Your cookies have expired. Please reset them and the PO Token";
        //	error.Error = EYtdlpError::CookiesExpired;
        //  return error;
        //}

        // This shouldnt happen but return just incase
        // TODO: Fix this damn issue, it happens a lot so maybe have multiple download attempts or just figure out a workaround to this
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
        std::cout << "CHECK FOR COOKIES IN YTDLP ERROR" << std::endl;

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