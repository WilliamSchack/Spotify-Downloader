#include "MusicScreenGUI.h"

MusicScreenGUI::MusicScreenGUI(QObject* parent) : QObject(parent)
{
    _addTracksPopup = new AddTracksPopup(this);
    connect(_addTracksPopup, &AddTracksPopup::DownloadRequested, this, &MusicScreenGUI::OnDownloadRequested);

    // Frequently check download events
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MusicScreenGUI::CheckDownloadEvents);

    timer->start(DOWNLOAD_EVENTS_POLL_INTERVAL_MS);
}

AddTracksPopup* MusicScreenGUI::GetAddTracksPopup() const
{
    return _addTracksPopup;
}

void MusicScreenGUI::OnDownloadRequested(const std::string& link, const std::string& destinationFolder)
{
    std::cout << link << " || " << destinationFolder << std::endl;

    bool downloadStarted = _downloader.RequestDownload(link, destinationFolder);
    if (!downloadStarted)
        return;

    _addTracksPopup->SetVisible(false);
}

void MusicScreenGUI::CheckDownloadEvents()
{
    if (!_downloader.IsDownloading())
        return;

    DownloadEventsQueue& downloadEvents = _downloader.GetEvents();
    while (std::optional<DownloadEvent> event = downloadEvents.TryGetEvent()) {
        std::visit(Overloaded {
            [&](const TrackProgressEvent& e) {

            },
            [&](const TrackSucceededEvent& e) {

            },
            [&](const TrackFailedEvent& e) {

            },
            [&](const DownloadStartedEvent& e) {

            },
            [&](const DownloadFailedToStartEvent& e) {

            },
            [&](const DownloadsFinishedEvent& e) {

            }
        }, event.value());
    }
}