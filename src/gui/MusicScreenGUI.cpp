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

    bool downloadStarted = _downloader.Download(link, destinationFolder);
    if (!downloadStarted)
        return;

    _addTracksPopup->SetVisible(false);
}

void MusicScreenGUI::CheckDownloadEvents()
{
    if (!_downloader.IsDownloading())
        return;

    while (std::optional<DownloadEvent> eventOpt = _downloader.GetEvents().TryGetEvent()) {
        if (!eventOpt.has_value())
            break;

        DownloadEvent event = eventOpt.value();
        std::cout << "MAIN THREAD GOT EVENTS >> " << event.Type << " " << event.TrackId << " " << event.Progress << " " << event.Message << std::endl;
    }
}