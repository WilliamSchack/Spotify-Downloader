#ifndef TEMPORARYPATHS_H
#define TEMPORARYPATHS_H

#include "FileUtils.h"
#include "TrackData.h"
#include "ICodec.h"

class TemporaryPaths
{
    public:
        static std::filesystem::path GetTempDir();
        static std::filesystem::path GetDownloadsDir();
        static std::filesystem::path GetImagesDir();
        static std::filesystem::path GetTrackDownloadPath(const TrackData& track, const ICodec& codec);
        static std::filesystem::path GetTrackImagePath(const TrackData& track);
    private:
        static inline const std::string DOWNLOADS_FOLDER_NAME = "Downloads";
        static inline const std::string IMAGES_FOLDER_NAME = "CoverArt";
};

#endif