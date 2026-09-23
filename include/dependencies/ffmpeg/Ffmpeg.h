#ifndef FFMPEG_H
#define FFMPEG_H

#include "FfmpegAudioDetails.h"
#include "ExternalProcess.h"
#include "CodecFactory.h"
#include "StringUtils.h"

#include <regex>
#include <filesystem>
#include <variant>

class Ffmpeg
{
    public:
        static FfmpegAudioDetails GetAudioDetails(const std::filesystem::path& filePath, const bool& getVolumeDetails = true);

        static std::filesystem::path Convert(const std::filesystem::path& currentPath, const EExtension& newExtension, const bool& deleteOriginal = true, std::function<void(float)> progressCallback = nullptr);

        // TODO: Add better normalisation
        // > Details already gotten in GetAudioDetails
        // > https://github.com/WilliamSchack/Spotify-Downloader/issues/101)
        static bool Normalise(const std::filesystem::path& filePath, const float& targetDb, std::function<void(float)> progressCallback = nullptr);
        static bool SetBitrate(const std::filesystem::path& filePath, const unsigned int& bitrate, std::function<void(float)> progressCallback = nullptr);
    private:
        static std::string Execute(const FfmpegAudioDetails& audioDetails, const std::vector<std::string>& args, const std::function<void(float)>& progressCallback = nullptr);
};

#endif