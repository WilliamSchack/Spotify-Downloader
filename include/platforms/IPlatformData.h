#ifndef IPLATFORMDATA_H
#define IPLATFORMDATA_H

#include "TrackData.h"

#include <filesystem>

class IPlatformData
{
    public:
        virtual ~IPlatformData() = default;

        // Returns error reason if it cant be downloaded, empty string if it can be downloaded
        virtual std::string GetTrackDownloadError(const TrackData& track) = 0;

        virtual std::vector<std::string> GetExtraDownloadArgs(const TrackData& track) = 0;
};

#endif