#include "Ffmpeg.h"

#include <iostream>

FfmpegAudioDetails Ffmpeg::GetAudioDetails(const std::filesystem::path& filePath, const bool& getVolumeDetails)
{
    Process process = Process::GetRelativeProcess(FFMPEG_PATH_RELATIVE);
    process.AddArgument("-i", "\"" + filePath.string() + "\"");
    if (getVolumeDetails) process.AddArgument("-af", "volumedetect,ebur128=peak=true:framelog=verbose");
    process.AddArgument("-f", "null"); // No format
    process.AddArgument("-"); // No output
    std::string output = process.Execute();

    // Parse details
    FfmpegAudioDetails details;
    
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

std::filesystem::path Ffmpeg::Convert(const std::filesystem::path& currentPath, const EExtension& newExtension)
{
    if (!std::filesystem::exists(currentPath))
        return "";

    // Get the new path and details
    std::unique_ptr<ICodec> targetCodec = CodecFactory::Create(newExtension);
    
    std::filesystem::path newPath = currentPath;
    newPath.replace_extension(targetCodec->GetString());

    if (std::filesystem::exists(newPath))
        return newPath;

    // Convert
    FfmpegAudioDetails audioDetails = GetAudioDetails(currentPath, false);
    std::vector<std::string> args {
        "-i", "\"" + currentPath.string() + "\"",
        "-progress", "-",
        "-nostats",
        targetCodec->GetFfmpegConversionParams()
    };

    if (!targetCodec->GetBitrateDetails().LockedBirtate && audioDetails.Bitrate > 0) {
        args.push_back("-b:a");
        args.push_back(std::to_string(audioDetails.Bitrate) + "k");
    }

    args.push_back("\"" + newPath.string() + "\"");

    Execute(audioDetails, args);

    return newPath;
}

bool Ffmpeg::Normalise(const std::filesystem::path& filePath, const float& targetDb)
{
    if (!std::filesystem::exists(filePath))
        return false;

    // Get details
    FfmpegAudioDetails audioDetails = GetAudioDetails(filePath); 
    std::unique_ptr<ICodec> codec = CodecFactory::Create(filePath.extension());

    std::filesystem::path tempPath = filePath;
    tempPath.replace_filename(filePath.stem().string() + "_ffmpeg" + filePath.extension().string());

    float dbDifference = (targetDb - audioDetails.MeanDB) + 0.4;  // Adding 0.4 here since normalized is always average 0.4-0.5 off of normalized target IDK why

    // Normalise
    std::vector<std::string> args {
        "-i", "\"" + filePath.string() + "\"",
        "-progress", "-",
        "-nostats",
        "-af", "volume=" + std::to_string(dbDifference) + "dB"
    };

    if (!codec->GetBitrateDetails().LockedBirtate && audioDetails.Bitrate > 0) {
        args.push_back("-b:a");
        args.push_back(std::to_string(audioDetails.Bitrate) + "k");
    }

    args.push_back("\"" + tempPath.string() + "\"");

    Execute(audioDetails, args);

    // Rename back to original
    if (!std::filesystem::exists(tempPath))
        return false;

    std::filesystem::remove(filePath);
    std::filesystem::rename(tempPath, filePath);

    return true;
}

bool Ffmpeg::SetBitrate(const std::filesystem::path& filePath, const unsigned int& bitrate)
{
    if (!std::filesystem::exists(filePath))
        return false;

    // Get details
    std::filesystem::path tempPath = filePath;
    tempPath.replace_filename(filePath.stem().string() + "_ffmpeg" + filePath.extension().string());
    
    // Set bitrate
    FfmpegAudioDetails audioDetails = GetAudioDetails(filePath, false);
    Execute(audioDetails, {
        "-i", "\"" + filePath.string() + "\"",
        "-progress", "-",
        "-nostats",
        "-b:a", std::to_string(bitrate) + "k",
        "\"" + tempPath.string() + "\""
    });

    // Rename back to original
    if (!std::filesystem::exists(tempPath))
        return false;

    std::filesystem::remove(filePath);
    std::filesystem::rename(tempPath, filePath);

    return true;
}

std::string Ffmpeg::Execute(const FfmpegAudioDetails& audioDetails, const std::vector<std::string>& args)
{
    // Get the progress
    std::function<void(std::string)> newLineCallback = [&](std::string line) {
        std::cout << line << std::endl;

        if (!StringUtils::Contains(line, "out_time_ms"))
            return;

        std::smatch matches;
        if (!std::regex_search(line, matches, std::regex(R"(=(\d+))")))
            return;

        int msProgress = std::stoi(matches[1]) / 1000;
        float progressPercent = (float)msProgress / (float)audioDetails.DurationMilliseconds;

        // Output this to a progress callback
        std::cout << progressPercent << std::endl;
    };

    // Setup command
    Process process = Process::GetRelativeProcess(FFMPEG_PATH_RELATIVE);
    for (std::string arg : args) {
        process.AddArgument(arg);
    }

    // Execute command
    return process.Execute(newLineCallback);
}