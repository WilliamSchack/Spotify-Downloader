#include "DownloadEventsQueue.h"

void DownloadEventsQueue::Send(DownloadEvent event)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _events.push(event);
}

void DownloadEventsQueue::ClearAll()
{
    std::queue<DownloadEvent> empty;
    std::swap(_events, empty);
}

std::optional<DownloadEvent> DownloadEventsQueue::TryGetEvent()
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (_events.empty())
        return std::nullopt;

    DownloadEvent event = std::move(_events.front());
    _events.pop();

    return event;
}