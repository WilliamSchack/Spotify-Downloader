#include "Ffmpeg.h"

#include <iostream>

FfmpegTrackDetails Ffmpeg::GetBasicDetails(const std::filesystem::path& filePath)
{
    Process process = Process::GetRelativeProcess(FFMPEG_PATH_RELATIVE);
    process.AddArgument("-i", "\"" + filePath.string() + "\"");
    std::string output = process.Execute();

    // Parse details
    FfmpegTrackDetails details;
    
    std::smatch matches;
    if (std::regex_search(output, matches, std::regex(R"(Duration:\s(.+?),)"))) {
        details.DurationMilliseconds = StringUtils::TimeToMilliseconds(matches[1]);
        details.DurationSeconds = details.DurationMilliseconds / 1000.0;
    }

    if (std::regex_search(output, matches, std::regex(R"(bitrate:\s(\d+?)\s)")))
        details.Bitrate = std::stoi(matches[1]);

    return details;
}

std::filesystem::path Ffmpeg::Convert(const std::filesystem::path& currentPath, const EExtension& newExtension)
{    
    // Get the new path and details
    std::unique_ptr<ICodec> targetCodec = CodecFactory::Create(newExtension);
    
    std::filesystem::path newPath = currentPath;
    newPath.replace_extension(targetCodec->GetString());

    if (std::filesystem::exists(newPath))
        return newPath;

    FfmpegTrackDetails trackDetails = GetBasicDetails(currentPath);

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