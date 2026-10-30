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

QList<QObject*> MusicScreenGUI::GetTracks() const
{
    return _tracks;
}

void MusicScreenGUI::OnDownloadRequested(const std::string& link, const std::string& destinationFolder)
{
    bool downloadDispatched = _downloader.RequestDownload(link, destinationFolder);
    if (!downloadDispatched) {
        _addTracksPopup->SetCheckingInputs(false);
        _addTracksPopup->SetErrorMessage("Failed to dispatch download... (This should not happen)");
        return;
    }

    _addTracksPopup->SetCheckingInputs(true);
    _addTracksPopup->SetErrorMessage("");
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
                
                _addTracksPopup->SetCheckingInputs(false);
                _addTracksPopup->SetErrorMessage("");
                _addTracksPopup->SetVisible(false);

                emit RequestNotification("Starting the download!");
            },
            [&](const DownloadFailedToStartEvent& e) {
                _addTracksPopup->SetCheckingInputs(false);
                _addTracksPopup->SetErrorMessage("Error: " + QString::fromStdString(e.Reason));
            },
            [&](const DownloadsFinishedEvent& e) {
                if (e.SuccessfulDownloads > 0 && Config::AutoOpenDownloadFolder)
                    QDesktopServices::openUrl(QUrl("file:///" + QString::fromStdString(e.OutputFolder)));
                
                std::string notificationMessage = std::to_string(e.SuccessfulDownloads) + " Downloads complete ";
                if (e.FailedDownloads > 0)
                    notificationMessage += "with " + std::to_string(e.FailedDownloads) + " errors..."; 
                else
                    notificationMessage += "with no errors!";

                emit RequestNotification(notificationMessage);
            }
        }, event.value());
    }
}