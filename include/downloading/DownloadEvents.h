#ifndef DOWNLOADEVENTS_H
#define DOWNLOADEVENTS_H

#include <string>
#include <queue>
#include <mutex>
#include <optional>

enum EDownloadEventType
{
    TrackProgress,
    TrackSucceeded,
    TrackFailed,
    ThreadFinished
};

struct DownloadEvent
{
    EDownloadEventType Type;
    
    // May change to internal id that is unique to each track to make it easier to identify which one is being used in the gui
    std::string TrackId;

    float Progress;
    std::string Message;
};

class DownloadEvents
{
    public:
        void Send(DownloadEvent event);
        std::optional<DownloadEvent> TryGetEvent();
    private:
        std::queue<DownloadEvent> _events;
        std::mutex _mutex;

};

#endif