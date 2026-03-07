#ifndef TRACKDOWNLOADER_H
#define TRACKDOWNLOADER_H

#include "Config.h"
#include "ELinkType.h"
#include "CodecFactory.h"
#include "PlatformFactory.h"
#include "PlatformUtils.h"
#include "ImageHandler.h"
#include "MetadataManager.h"
#include "Ytdlp.h"
#include "Ffmpeg.h"
#include "LyricsFinder.h"
#include "FilePathReserver.h"

#include <iostream>

class TrackDownloader
{
    public:
        static bool DownloadTrack(const TrackData& track, const EPlatform& searchPlatform, const std::string& directory);
        static int DownloadTracks(const std::vector<TrackData>& tracks, const EPlatform& searchPlatform, const std::string& directory);
    private:
        static std::filesystem::path FindAvailableTrackPath(const TrackData& track, const std::filesystem::path& originalPath);
    private:
        static inline const std::string DOWNLOADS_FOLDER_NAME = "Downloads";
        static inline const std::string IMAGES_FOLDER_NAME = "CoverArt";
};

#endif