#ifndef FILEPATHRESERVER_H
#define FILEPATHRESERVER_H

#include "MetadataManager.h"
#include "VectorUtils.h"
#include "TrackData.h"

#include <filesystem>
#include <mutex>

class FilePathReserver
{
    public:
        ~FilePathReserver();

        std::filesystem::path FindAvailableTrackPath(const TrackData& track, const std::filesystem::path& originalPath);
    private:
        void SetPath(const std::filesystem::path& path);
    private:
        inline static std::mutex _reservingPathMutex;
        inline static std::vector<std::filesystem::path> _reservedPaths;

        std::filesystem::path _path;
};

#endif