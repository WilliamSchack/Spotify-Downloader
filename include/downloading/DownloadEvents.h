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

struct DownloadFailedToStartEvent {};

struct DownloadsFinishedEvent {};

using DownloadEvent = std::variant<
    TrackProgressEvent,
    TrackSucceededEvent,
    TrackFailedEvent,
    DownloadStartedEvent,
    DownloadFailedToStartEvent,
    DownloadsFinishedEvent
>;

#endif