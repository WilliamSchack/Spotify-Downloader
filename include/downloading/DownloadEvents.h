#ifndef DOWNLOADEVENTS_H
#define DOWNLOADEVENTS_H

#include "TrackData.h"

#include <string>
#include <variant>
#include <vector>


struct TrackProgressEvent
{
    std::string TrackUniqueId;
    float Progress;
    std::string Message;
};

struct TrackCoverArtDownloadedEvent
{
    std::string TrackUniqueId;
    std::filesystem::path FilePath;
};

struct TrackSucceededEvent
{
    std::string TrackUniqueId;
};

struct TrackFailedEvent
{
    std::string TrackUniqueId;
    std::string Reason;
};

struct DownloadStartedEvent
{
    std::vector<TrackData> Tracks;
};

struct DownloadFailedToStartEvent
{
    std::string Reason;
};

struct DownloadsFinishedEvent
{
    int FailedDownloads;
    int SuccessfulDownloads;
    std::string OutputFolder;
};

using DownloadEvent = std::variant<
    TrackProgressEvent,
    TrackCoverArtDownloadedEvent,
    TrackSucceededEvent,
    TrackFailedEvent,
    DownloadStartedEvent,
    DownloadFailedToStartEvent,
    DownloadsFinishedEvent
>;

#endif