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
    // Get the new path and details
    std::unique_ptr<ICodec> targetCodec = CodecFactory::Create(newExtension, false);
    
    std::filesystem::path newPath = currentPath;
    newPath.replace_extension(targetCodec->GetString());

    if (std::filesystem::exists(newPath))
        return newPath;

    FfmpegAudioDetails trackDetails = GetAudioDetails(currentPath);

    std::function<void(std::string)> newLineCallback = [&](std::string line) {
        // Get the conversion progress
        if (!StringUtils::Contains(line, "out_time_ms"))
            return;

        std::smatch matches;
        if (!std::regex_search(line, matches, std::regex(R"(=(\d+))")))
            return;

        int msProgress = std::stoi(matches[1]) / 1000;
        float progressPercent = (float)msProgress / (float)trackDetails.DurationMilliseconds;

        // Output this to a progress callback
        std::cout << progressPercent << std::endl;
    };

    // Convert
    Process process = Process::GetRelativeProcess(FFMPEG_PATH_RELATIVE);
    process.AddArgument("-i", "\"" + currentPath.string() + "\"");
    process.AddArgument("-progress", "-");
    process.AddArgument("-nostats");
    process.AddArgument(targetCodec->GetFfmpegConversionParams());
    if (!targetCodec->GetBitrateDetails().LockedBirtate && trackDetails.Bitrate > 0)
        process.AddArgument("-b:a", std::to_string(trackDetails.Bitrate) + "k");
    process.AddArgument("\"" + newPath.string() + "\"");
    
    process.Execute(newLineCallback);

    return newPath;
}