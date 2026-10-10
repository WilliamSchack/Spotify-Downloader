#include "MusicScreenGUI.h"

MusicScreenGUI::MusicScreenGUI(QWindow* mainWindow, QObject* parent) : QObject(parent)
{
    _addTracksPopup = new AddTracksPopup(mainWindow, this);
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

QList<QObject*> MusicScreenGUI::GetTracks() const
{
    return _tracks;
}

void MusicScreenGUI::OnDownloadRequested(const std::string& link, const std::string& destinationFolder)
{
    std::cout << link << " || " << destinationFolder << std::endl;

    bool downloadDispatched = _downloader.RequestDownload(link, destinationFolder);
    if (!downloadDispatched)
        return;
}

void MusicScreenGUI::CheckDownloadEvents()
{
    DownloadEventsQueue& downloadEvents = _downloader.GetEvents();
    while (std::optional<DownloadEvent> event = downloadEvents.TryGetEvent()) {
        std::visit(Overloaded {
            [&](const TrackProgressEvent& e) {
                TrackUI* track = _tracksFromId[e.TrackUniqueId];
                track->SetProgress(e.Progress);
                track->SetStatus(e.Message);
            },
            [&](const TrackCoverArtDownloadedEvent& e) {
                _tracksFromId[e.TrackUniqueId]->SetCoverArtUrl(e.FilePath);
            },
            [&](const TrackSucceededEvent& e) {
                TrackUI* track = _tracksFromId[e.TrackUniqueId];
                track->SetProgress(1.0);
                track->SetStatus("Download Succeeded!");
            },
            [&](const TrackFailedEvent& e) {
                TrackUI* track = _tracksFromId[e.TrackUniqueId];
                track->SetProgress(1.0);
                track->SetStatus("FAILED: " + e.Reason);
            },
            [&](const DownloadStartedEvent& e) {
                // Remove old tracks
                for (QObject* oldTrack : _tracks)
                    oldTrack->deleteLater();
                    
                _tracks.clear();
                _tracksFromId.clear();

                // Add new tracks
                for (const TrackData& trackData : e.Tracks) {
                    TrackUI* trackUI = new TrackUI(trackData, this);
                    _tracks.push_back(trackUI);
                    _tracksFromId.insert({trackData.GetUniqueId(), trackUI});
                }

                emit TracksChanged();
                
                _addTracksPopup->SetVisible(false);
            },
            [&](const DownloadFailedToStartEvent& e) {
                // Show error popup
            },
            [&](const DownloadsFinishedEvent& e) {
                if (e.SuccessfulDownloads > 0 && Config::AutoOpenDownloadFolder)
                    QDesktopServices::openUrl(QUrl(QString::fromStdString(e.OutputFolder)));
            }
        }, event.value());
    }
}