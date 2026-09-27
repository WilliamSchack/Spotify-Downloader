#include "DownloadEvents.h"

void DownloadEvents::Send(DownloadEvent event)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _events.push(event);
}

void DownloadEvents::ClearAll()
{
    std::queue<DownloadEvent> empty;
    std::swap(_events, empty);
}

std::optional<DownloadEvent> DownloadEvents::TryGetEvent()
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (_events.empty())
        return std::nullopt;

    DownloadEvent event = _events.front();
    _events.pop();

    return event;
}