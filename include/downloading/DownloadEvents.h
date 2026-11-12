#ifndef DOWNLOADEVENTS_H
#define DOWNLOADEVENTS_H

#include "TrackData.h"
#include "EPlatform.h"

#include <string>
#include <variant>
#include <vector>

struct TrackProgressEvent
{
    std::string TrackUniqueId = "";
    float Progress = 0.0f;
    std::string Message = "";
};

struct TrackCoverArtDownloadedEvent
{
    std::string TrackUniqueId = "";
    std::filesystem::path FilePath = "";
};

struct TrackSourceFoundEvent
{
    std::string TrackUniqueId = "";
    EPlatform Platform = EPlatform::Unknown;
    std::string Link;
};

struct TrackSucceededEvent
{
    std::string TrackUniqueId = "";
};

struct TrackFailedEvent
{
    std::string TrackUniqueId = "";
    std::string Reason = "";
};

struct DownloadStartedEvent
{
    std::vector<TrackData> Tracks;
};

struct DownloadFailedToStartEvent
{
    std::string Reason = "";
};

struct DownloadsFinishedEvent
{
    int FailedDownloads = 0;
    int SuccessfulDownloads = 0;
    std::string OutputFolder = "";
};

struct DownloadsCancelledEvent
{
    int FailedDownloads = 0;
    int SuccessfulDownloads = 0;
    std::string OutputFolder = "";
};

struct DownloadsPausedEvent {};

using DownloadEvent = std::variant<
    TrackProgressEvent,
    TrackCoverArtDownloadedEvent,
    TrackSourceFoundEvent,
    TrackSucceededEvent,
    TrackFailedEvent,
    DownloadStartedEvent,
    DownloadFailedToStartEvent,
    DownloadsFinishedEvent,
    DownloadsCancelledEvent,
    DownloadsPausedEvent
>;

#endif