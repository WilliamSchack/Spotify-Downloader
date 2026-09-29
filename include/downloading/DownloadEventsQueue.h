#ifndef DOWNLOADEVENTSQUEUE_H
#define DOWNLOADEVENTSQUEUE_H

#include "DownloadEvents.h"

#include <queue>
#include <mutex>
#include <optional>

class DownloadEventsQueue
{
    public:
        void Send(DownloadEvent event);
        void ClearAll();
        std::optional<DownloadEvent> TryGetEvent();
    private:
        std::queue<DownloadEvent> _events;
        std::mutex _mutex;

};

#endif