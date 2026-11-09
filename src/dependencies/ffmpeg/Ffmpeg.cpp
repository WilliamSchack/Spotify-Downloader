#include "Ffmpeg.h"

bool Ffmpeg::DownloadExecutableImpl(std::filesystem::path path)
{
    // Get the download details
    std::string url = "";
    std::string archiveName = "";
    std::string pathInArchive = "";

#ifdef WIN32
    url = "https://github.com/BtbN/FFmpeg-Builds/releases/download/latest/";
    archiveName = "ffmpeg-master-latest-win";
    
    #ifdef IS_ARM_CPU
        archiveName += "arm64-lgpl";
    #else
        archiveName += "64-lgpl";
    #endif

    url += archiveName + ".zip";
    pathInArchive = archiveName + "/bin/ffmpeg.exe";
#elif __APPLE__
    // Get download url from details json
    NetworkRequest appleDetailsRequest;
	request.Url = "https://evermeet.cx/ffmpeg/info/ffmpeg/release";
    NetworkResponse appleDetailsResponse = appleDetailsRequest.Get();

    nlohmann::json appleResponseJson = nlohmann::json::parse(appleDetailsResponse.Body);
    url = appleResponseJson["download"]["zip"]["url"];
    archiveName = "ffmpeg.zip"
    pathInArchive = "ffmpeg";
#else
    url = "https://github.com/BtbN/FFmpeg-Builds/releases/download/latest/";
    archiveName = "ffmpeg-master-latest-linux";

    #ifdef IS_ARM_CPU
        archiveName += "arm64-lgpl";
    #else
        archiveName += "64-lgpl";
    #endif

    url += archiveName + ".tar.xz";
    pathInArchive = archiveName + "/bin/ffmpeg";
#endif

    // Download
    std::filesystem::path archivePath = Paths::GetTempDir() / archiveName;

    NetworkRequest request;
    request.Url = url;
    request.DownloadToFile(archivePath);

    ArchiveHandler::ExtractFile(archivePath, pathInArchive, path);

    std::filesystem::remove(archivePath);

    return std::filesystem::exists(path);
}

bool Ffmpeg::UpdateExecutableImpl()
{
    std::filesystem::path path = GetPath();
    if (path.empty())
        return false;

    std::filesystem::path tempPath = Paths::GetTempDir() / "ffmpeg-temp";
    bool downloaded = DownloadExecutableImpl(tempPath);
    if (!downloaded)
        return false;

    // Replace the current executable with the updated one
    std::filesystem::path targetPath = FileUtils::GetRelativePath(GetRelativePath());
    if (std::filesystem::exists(targetPath))
        std::filesystem::remove(targetPath);

    bool fileMoved = FileUtils::MoveFile(tempPath, targetPath);
    if (!fileMoved)
        return false;

    return true;
}

std::string Ffmpeg::GetVersionImpl()
{
    std::filesystem::path path = GetPath();
    if (path.empty())
        return "";

    ExternalProcess process(path);
    process.AddArgument("-version");

    std::string output = process.Execute().Output;

    if (!IsInstalledRelative()) {
        // Dont check for specific version string if installed on the system
        std::smatch matches;
        if (!std::regex_search(output, matches, std::regex(R"(version\s(.*?\d)\s)")))
            return "";

        return matches[1];
    }

#ifdef __APPLE__
    std::smatch matches;
    if (!std::regex_search(output, matches, std::regex(R"(version\s(\d+\.\d+\.\d+))")))
        return "";

    return matches[1];
#else
    std::smatch matches;
    if (!std::regex_search(output, matches, std::regex(R"(--extra-version=(.+)\s)")))
        return "";

    // Reformat version from yyyymmdd > yyyy-mm-dd
    std::string versionOutput = matches[1];
    return versionOutput.substr(0, 4) + "-" + versionOutput.substr(4, 2) + "-" + versionOutput.substr(6, 2);
#endif
}

std::string Ffmpeg::GetLatestVersionImpl()
{
#ifdef __APPLE__
    NetworkRequest request;
	request.Url = "https://evermeet.cx/ffmpeg/info/ffmpeg/release";
    NetworkResponse response = request.Get();

    if (response.CurlCode != CURLE_OK) {
        std::cout << "Failed to get the latest version: " << curl_easy_strerror(response.CurlCode) << std::endl;
        return "";
    }

    nlohmann::json json = nlohmann::json::parse(response.Body);
    return json["version"];
#else
    std::string latestTag = GitHubUtils::GetRecentTag("BtbN", "FFmpeg-Builds", 1);

    std::smatch matches;
    if (!std::regex_search(latestTag, matches, std::regex(R"(\D-(\d+-\d+-\d+)-)")))
        return "";
    
    return matches[1];
#endif
}

std::string Ffmpeg::GetFileNameImpl()
{
    return "ffmpeg";
}

FfmpegAudioDetails Ffmpeg::GetAudioDetails(const std::filesystem::path& filePath, const bool& getVolumeDetails)
{
    FfmpegAudioDetails details;

    std::filesystem::path ffmpegPath = GetPath();
    if (ffmpegPath.empty()) {
        std::cout << "ffmpeg is not installed..." << std::endl;
        return details;
    }

    ExternalProcess process(ffmpegPath);
    process.AddArgument("-i", FileUtils::PathToUtf8(filePath));

    if (getVolumeDetails) process.AddArgument("-af", "volumedetect,ebur128=peak=true:framelog=verbose");
    process.AddArgument("-f", "null"); // No format
    process.AddArgument("-"); // No output
    std::string output = process.Execute().Output;

    // Parse details    
    std::smatch matches;

    // Duration
    if (std::regex_search(output, matches, std::regex(R"(Duration:\s(.+?),)"))) {
        details.DurationMilliseconds = StringUtils::TimeToMilliseconds(matches[1]);
        details.DurationSeconds = details.DurationMilliseconds / 1000.0;
    }

    // Bitrate
    if (std::regex_search(output, matches, std::regex(R"(bitrate:\s(\d+?)\s)")))
        details.Bitrate = std::stoi(matches[1]);

    if (!getVolumeDetails)
        return details;

    // Decibals
    if (std::regex_search(output, matches, std::regex(R"(mean_volume:\s([^\s]+?)\s)")))
        details.MeanDB = std::stof(matches[1]);

    if (std::regex_search(output, matches, std::regex(R"(max_volume:\s([^\s]+?)\s)")))
        details.MaxDB = std::stof(matches[1]);

    // Integrated Loudness
    if (std::regex_search(output, matches, std::regex(R"(I:\s+?([^\s]+?)\s)")))
        details.IntegratedLoudnessLUFS = std::stof(matches[1]);

    if (std::regex_search(output, matches, std::regex(R"(loudness:[\S\s]+?Threshold:\s+?([^\s]+?)\s)")))
        details.IntegratedLoudnessThresholdLUFS = std::stof(matches[1]);

    // Loudness Range
    if (std::regex_search(output, matches, std::regex(R"(LRA:\s+?([^\s]+?)\s)")))
        details.LoudnessRangeLUFS = std::stof(matches[1]);

    if (std::regex_search(output, matches, std::regex(R"(range:[\S\s]+?Threshold:\s+?([^\s]+?)\s)")))
        details.LoudnessRangeThresholdLUFS = std::stof(matches[1]);

    if (std::regex_search(output, matches, std::regex(R"(LRA\slow:\s+?([^\s]+?)\s)")))
        details.LoudnessRangeLowLUFS = std::stof(matches[1]);

    if (std::regex_search(output, matches, std::regex(R"(LRA\shigh:\s+?([^\s]+?)\s)")))
        details.LoudnessRangeHighLUFS = std::stof(matches[1]);

    // True Peak
    if (std::regex_search(output, matches, std::regex(R"(Peak:\s+?([^\s]+?)\s)")))
        details.TruePeakDBFS = std::stof(matches[1]);

    return details;
}

std::filesystem::path Ffmpeg::Convert(const std::filesystem::path& filePath, const EExtension& newExtension, const bool& deleteOriginal, std::function<void(float)> progressCallback)
{
    if (!std::filesystem::exists(filePath))
        return "";

    // Get the new path and details
    std::unique_ptr<ICodec> targetCodec = CodecFactory::Create(newExtension);
    
    std::filesystem::path newPath = filePath;
    newPath.replace_extension(targetCodec->GetString());

    if (std::filesystem::exists(newPath))
        return newPath;

    // Convert
    FfmpegAudioDetails audioDetails = GetAudioDetails(filePath, false);
    std::vector<std::string> args {
        "-i", FileUtils::PathToUtf8(filePath),
        "-y",
        "-progress", "-",
        "-nostats"
    };

    for (const std::string& param : targetCodec->GetFfmpegConversionParams()) {
        args.push_back(param);
    }

    if (!targetCodec->GetBitrateDetails().LockedBirtate && audioDetails.Bitrate > 0) {
        args.push_back("-b:a");
        args.push_back(std::to_string(audioDetails.Bitrate) + "k");
    }

    args.push_back(FileUtils::PathToUtf8(newPath));

    Execute(audioDetails, args, progressCallback);

    if (deleteOriginal)
        std::filesystem::remove(filePath);

    return newPath;
}

bool Ffmpeg::Normalise(const std::filesystem::path& filePath, const float& targetDb, std::function<void(float)> progressCallback)
{
    if (!std::filesystem::exists(filePath))
        return false;

    // Get details
    FfmpegAudioDetails audioDetails = GetAudioDetails(filePath); 
    std::unique_ptr<ICodec> codec = CodecFactory::Create(filePath.extension().string());

    std::filesystem::path tempPath = filePath;
    tempPath.replace_filename(FileUtils::PathToUtf8(filePath.stem()) + "_ffmpeg" + FileUtils::PathToUtf8(filePath.extension()));

    float dbDifference = (targetDb - audioDetails.MeanDB) + 0.4;  // Adding 0.4 here since normalized is always average 0.4-0.5 off of normalized target IDK why

    // Normalise
    std::vector<std::string> args {
        "-i", FileUtils::PathToUtf8(filePath),
        "-y",
        "-progress", "-",
        "-nostats",
        "-af", "volume=" + std::to_string(dbDifference) + "dB"
    };

    if (!codec->GetBitrateDetails().LockedBirtate && audioDetails.Bitrate > 0) {
        args.push_back("-b:a");
        args.push_back(std::to_string(audioDetails.Bitrate) + "k");
    }

    args.push_back(FileUtils::PathToUtf8(tempPath.string()));

    Execute(audioDetails, args, progressCallback);

    // Rename back to original
    if (!std::filesystem::exists(tempPath))
        return false;

    std::filesystem::remove(filePath);
    std::filesystem::rename(tempPath, filePath);

    return true;
}

bool Ffmpeg::SetBitrate(const std::filesystem::path& filePath, const unsigned int& bitrate, std::function<void(float)> progressCallback)
{
    if (!std::filesystem::exists(filePath))
        return false;

    // Get details
    std::filesystem::path tempPath = filePath;
    tempPath.replace_filename(FileUtils::PathToUtf8(filePath.stem()) + "_ffmpeg" + FileUtils::PathToUtf8(filePath.extension()));
    
    // Set bitrate
    FfmpegAudioDetails audioDetails = GetAudioDetails(filePath, false);
    Execute(audioDetails, {
        "-i", FileUtils::PathToUtf8(filePath),
        "-y",
        "-progress", "-",
        "-nostats",
        "-b:a", std::to_string(bitrate) + "k",
        FileUtils::PathToUtf8(tempPath)
    }, progressCallback);

    // Rename back to original
    if (!std::filesystem::exists(tempPath))
        return false;

    std::filesystem::remove(filePath);
    std::filesystem::rename(tempPath, filePath);

    return true;
}

std::string Ffmpeg::Execute(const FfmpegAudioDetails& audioDetails, const std::vector<std::string>& args, const std::function<void(float)>& progressCallback)
{
    std::filesystem::path ffmpegPath = GetPath();
    if (ffmpegPath.empty()) {
        std::cout << "ffmpeg is not installed..." << std::endl;
        return "";
    }

    // Get the progress
    std::function<void(std::string)> newLineCallback = [&](std::string line) {
        if (progressCallback == nullptr)
            return;

        if (!StringUtils::Contains(line, "out_time_ms"))
            return;

        std::smatch matches;
        if (!std::regex_search(line, matches, std::regex(R"(=(\d+))")))
            return;

        int msProgress = std::stoi(matches[1]) / 1000;
        float progressPercent = (float)msProgress / (float)audioDetails.DurationMilliseconds;
        progressCallback(progressPercent);
    };

    // Setup command
    ExternalProcess process(ffmpegPath);
    for (const std::string& arg : args)
        process.AddArgument(arg);

    // Execute command
    return process.Execute(0, newLineCallback).Output;
}