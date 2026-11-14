#include "MusicScreenGUI.h"

MusicScreenGUI::MusicScreenGUI(QObject* parent) : QObject(parent)
{
    _addTracksPopup = new AddTracksPopup(this);
    connect(_addTracksPopup, &AddTracksPopup::DownloadRequested, this, &MusicScreenGUI::OnDownloadRequested);

    // Update background image
    if (Config::RandomDefaultBackground) {
        int randomIndex = std::rand() % Config::RANDOM_DEFAULT_BACKGROUND_URLS.size();
        UpdateBackgroundImage(Config::RANDOM_DEFAULT_BACKGROUND_URLS[randomIndex]);
    } else {
        UpdateBackgroundImage();
    }


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

    if (Config::DefaultSaveLocation.empty()) {
        Config::DefaultSaveLocation = FileUtils::PathFromUtf8(destinationFolder);
    }
}

void MusicScreenGUI::DownloadButtonClicked()
{
    if (_downloader.IsStopping())
        return;
    
    // Cancel
    if (_downloader.IsDownloading()) {
        // TODO: Add confirmation popup
        
        std::thread([this]() {
            // Might need to wait for the lock so may aswell do it in another thread 
            _downloader.Stop(); 
            
            QMetaObject::invokeMethod(this, [this]() {
                emit DownloadButtonTextChanged();
            }, Qt::QueuedConnection);
        }).detach();

        return;
    }

    // Start new download
    _addTracksPopup->SetLinkInputText("");
    _addTracksPopup->SetErrorMessage("");
    _addTracksPopup->SetVisible(true);
}

void MusicScreenGUI::PlayPauseButtonClicked()
{
    if (!_downloader.IsDownloading())
        return;

    if (_downloader.IsPausing() || _downloader.IsPaused())
        _downloader.Resume();
    else
        _downloader.Pause();

    emit PlayPauseButtonTextChanged();
}

void MusicScreenGUI::BackgroundApplyButtonClicked()
{
    UpdateBackgroundImage();
}

QString MusicScreenGUI::GetDownloadButtonText() const
{
    if (_downloader.IsStopping())
        return "Cancelling...";
    
    if (_downloader.IsDownloading())
        return "Cancel Download";

    return "Start New Download";
}

QString MusicScreenGUI::GetPlayPauseButtonText() const
{
    if (!_downloader.IsDownloading())
        return "Pause";

    if (_downloader.IsPaused())
        return "Resume";
    
    if (_downloader.IsPausing())
        return "Pausing...";

    return "Pause";
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
            [&](const TrackSourceFoundEvent& e) {
                TrackUI* track = _tracksFromId[e.TrackUniqueId];
                track->AddSource(e.Platform, e.Link);
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

                UpdateBackgroundImage(_downloader.GetPlaylistData());
                emit DownloadButtonTextChanged();
                emit RequestNotification("Starting the download!");
            },
            [&](const DownloadFailedToStartEvent& e) {
                _addTracksPopup->SetCheckingInputs(false);
                _addTracksPopup->SetErrorMessage("Error: " + QString::fromStdString(e.Reason));
            },
            [&](const DownloadsFinishedEvent& e) {
                if (e.SuccessfulDownloads > 0 && Config::AutoOpenDownloadFolder)
                    QDesktopServices::openUrl(QUrl::fromLocalFile(QString::fromStdString(e.OutputFolder)));
                
                std::string notificationMessage = std::to_string(e.SuccessfulDownloads) + " Downloads complete ";
                if (e.FailedDownloads > 0)
                    notificationMessage += "with " + std::to_string(e.FailedDownloads) + " errors..."; 
                else
                    notificationMessage += "with no errors!";

                emit DownloadButtonTextChanged();
                emit PlayPauseButtonTextChanged();
                emit RequestNotification(notificationMessage);
            },
            [&](const DownloadsCancelledEvent& e) {
                if (e.SuccessfulDownloads > 0 && Config::AutoOpenDownloadFolder)
                    QDesktopServices::openUrl(QUrl::fromLocalFile(QString::fromStdString(e.OutputFolder)));

                std::string notificationMessage = "Download cancelled with " + std::to_string(e.SuccessfulDownloads) + " downloads complete ";
                if (e.FailedDownloads > 0)
                    notificationMessage += "with " + std::to_string(e.FailedDownloads) + " errors..."; 
                else
                    notificationMessage += "with no errors!";

                emit DownloadButtonTextChanged();
                emit PlayPauseButtonTextChanged();
                emit RequestNotification(notificationMessage);
            },
            [&](const DownloadsPausedEvent& e) {
                emit PlayPauseButtonTextChanged();
            }
        }, event.value());
    }
}

void MusicScreenGUI::UpdateBackgroundImage(const PlaylistData& playlistData)
{
    SetLoadingBackgroundImage(true);

    std::thread([this, playlistData]() {
        std::filesystem::path tempImagesFolder = Paths::GetImagesDir();
        std::string fileName = playlistData.GetUniqueId();
        fileName = FileUtils::ValidateFileName(fileName);

        // Check if it exists
        std::filesystem::path imageFilePathNoExtension = tempImagesFolder / fileName;
        std::filesystem::path imageFilePath = FileUtils::FindPathWithAnyExtension(imageFilePathNoExtension.parent_path(), imageFilePathNoExtension.filename());
        if (imageFilePath.empty()) {
            // Download
            Image image = ImageHandler::DownloadImage(playlistData.ImageUrl);
            if (image.Data == nullptr) {
                DisableBackgroundImageLoadingSafely();
                return;
            }

            imageFilePath = ImageHandler::SaveImage(imageFilePathNoExtension, image);
            if (imageFilePath.empty()) {
                DisableBackgroundImageLoadingSafely();
                return;
            }
        }

        QString pathString = QString::fromStdString(FileUtils::PathToUtf8(imageFilePath));
        pathString = QUrl::fromLocalFile(pathString).toString();
        QMetaObject::invokeMethod(this, [this, pathString]() {
            SetBackgroundImagePath(pathString);
            SetLoadingBackgroundImage(false);
        }, Qt::QueuedConnection);
    }).detach();
}

void MusicScreenGUI::UpdateBackgroundImage(const std::string& url)
{
    SetLoadingBackgroundImage(true);

    std::thread([this, url]() {
        EPlatform platform = PlatformDetector::GetPlatformFromUrl(url);
        if (platform == EPlatform::Unknown) {
            DisableBackgroundImageLoadingSafely();
            return;
        }

        std::unique_ptr<IPlatformDownloader> platformDownloader = PlatformFactory::CreateDownloader(platform);
        if (platformDownloader == nullptr) {
            DisableBackgroundImageLoadingSafely();
            return;
        }

        ELinkType linkType = platformDownloader->GetLinkType(url);
        if (linkType == ELinkType::Unknown) {
            DisableBackgroundImageLoadingSafely();
            return;
        }

        switch (linkType) {
            case ELinkType::Track: {
                TrackData track = platformDownloader->GetTrack(url);

                PlaylistData playlistData(platform);
                playlistData.Id = track.Id;
                playlistData.ImageUrl = track.Album.ImageUrl;
                UpdateBackgroundImage(playlistData);

                break;
            } case ELinkType::Playlist: {
                PlaylistTracks playlist = platformDownloader->GetPlaylist(url);
                UpdateBackgroundImage(playlist.Data);

                break;
            } case ELinkType::Album: {
                AlbumTracks album = platformDownloader->GetAlbum(url);

                PlaylistData playlistData(platform);
                playlistData.Id = album.Data.Id;
                playlistData.ImageUrl = album.Data.ImageUrl;
                UpdateBackgroundImage(playlistData);

                break;
            }
        }
    }).detach();
}

void MusicScreenGUI::UpdateBackgroundImage()
{
    if (Config::DefaultBackgroundUrl.empty())
        return;

    UpdateBackgroundImage(Config::DefaultBackgroundUrl);
}

void MusicScreenGUI::DisableBackgroundImageLoadingSafely()
{
    // Just does it on the gui thread
    QMetaObject::invokeMethod(this, [this]() {
        SetLoadingBackgroundImage(false);
    }, Qt::QueuedConnection);
}