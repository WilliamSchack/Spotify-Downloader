#ifndef FFMPEG_H
#define FFMPEG_H

#include "FfmpegAudioDetails.h"
#include "Process.h"
#include "CodecFactory.h"
#include "StringUtils.h"

#include <regex>
#include <filesystem>

class Ffmpeg
{
    public:
        static FfmpegAudioDetails GetAudioDetails(const std::filesystem::path& filePath, const bool& getVolumeDetails = true);

        // Returns the new path
        static std::filesystem::path Convert(const std::filesystem::path& currentPath, const EExtension& newExtension);
};

#endif