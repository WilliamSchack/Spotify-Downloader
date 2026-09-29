#ifndef TRACKDOWNLOADER_H
#define TRACKDOWNLOADER_H

#include "Config.h"
#include "ELinkType.h"
#include "CodecFactory.h"
#include "PlatformFactory.h"
#include "PlatformUtils.h"
#include "MathUtils.h"
#include "ImageHandler.h"
#include "MetadataManager.h"
#include "Ytdlp.h"
#include "Ffmpeg.h"
#include "LyricsFinder.h"
#include "FilePathReserver.h"
#include "TemporaryPaths.h"
#include "DownloadProgress.h"
#include "DownloadResult.h"

#include <iostream>
#include <functional>

class TrackDownloader
{
    public:
        static DownloadResult DownloadTrack(const TrackData& track, const EPlatform& searchPlatform, const std::string& directory, std::function<void(DownloadProgress)> progressCallback = nullptr, std::function<void(std::filesystem::path)> coverArtDownloadedCallback = nullptr);
        static int DownloadTracks(const std::vector<TrackData>& tracks, const EPlatform& searchPlatform, const std::string& directory, std::function<void(int, DownloadProgress)> progressCallback = nullptr, std::function<void(int, std::filesystem::path)> coverArtDownloadedCallback = nullptr, std::function<void(int, DownloadResult)> trackDownloadedCallback = nullptr);
    private:
        static void SetProgress(const std::function<void(DownloadProgress)>& progressCallback, const DownloadProgress& progress);
};

#endif