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
        static std::filesystem::path GetTrackDownloadPathNoExtension(const TrackData& track);
        static std::filesystem::path GetTrackImagePathNoExtension(const TrackData& track);
        static std::filesystem::path FindExistingTrackImagePath(const TrackData& track);
    private:
        static inline const std::string DOWNLOADS_FOLDER_NAME = "Downloads";
        static inline const std::string IMAGES_FOLDER_NAME = "CoverArt";
};

#endif