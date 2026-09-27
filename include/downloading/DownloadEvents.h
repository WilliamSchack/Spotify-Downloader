#ifndef DOWNLOADEVENTS_H
#define DOWNLOADEVENTS_H

#include <string>

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
        void Process();
        static void ProcessAll();
        void Send(DownloadEvent event);
};

#endif