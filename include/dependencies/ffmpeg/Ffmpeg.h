#ifndef FFMPEG_H
#define FFMPEG_H

#include "FfmpegTrackDetails.h"
#include "Process.h"
#include "CodecFactory.h"
#include "StringUtils.h"

#include <regex>
#include <filesystem>

class Ffmpeg
{
    public:
        static FfmpegTrackDetails GetBasicDetails(const std::filesystem::path& filePath);

        // Returns the new path
        static std::filesystem::path Convert(const std::filesystem::path& currentPath, const EExtension& newExtension);
};

#endif