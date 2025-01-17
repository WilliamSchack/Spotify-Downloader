#include "SpotifyDownloader.h"

#include "Utilities/Animation.h"
#include "Utilities/MathUtils.h"

void SpotifyDownloader::SetupUI(int threadIndex) {
    _ui.GettingPlaylistDataLabel->hide();
    
    _downloaderUI.clear();
    for (int i = 0; i < threadIndex; i++) {
        DownloaderThread* currentUI;
        switch (Config::DownloaderThreadUIIndex) {
            case 0: // Compact
                currentUI = new CompactDownloaderThread();
                break;
            case 1: // Original
                currentUI = new OriginalDownloaderThread();
                break;
            case 2: // Dynamic
                // If original threads overflow scroll area use compact design
                if (threadIndex > 2)
                    currentUI = new CompactDownloaderThread();
                else
                    currentUI = new OriginalDownloaderThread();
                break;
        }


        _ui.DownloadingSpaceFillerLayout->insertWidget(_ui.DownloadingSpaceFillerLayout->count() - 1, currentUI);

        _downloaderUI.append(currentUI);
    }
}

int SpotifyDownloader::CurrentScreen() {
    return _ui.Screens->currentIndex();
}

void SpotifyDownloader::ChangeScreen(int screenIndex) {
    int currentScreen = CurrentScreen();

    // Animate Side Panel
    switch (screenIndex) {
        case Config::SETUP_SCREEN_INDEX:
        case Config::PROCESSING_SCREEN_INDEX:
            _ui.DownloadingScreenButton->setIcon(Config::DownloadIconFilled());
            if (_errorUI.count() > 0) _ui.ErrorScreenButton->setIcon(Config::ErrorIcon());
            _ui.SettingsScreenButton->setIcon(Config::SettingsIcon());
            Animation::AnimatePosition(_ui.SideBar_LineIndicator, QPoint(0, _ui.DownloadingScreenButton->y() - 2), 500);
            break;
        case Config::ERROR_SCREEN_INDEX:
            _ui.ErrorScreenButton->setIcon(Config::ErrorIconFilled());
            _ui.DownloadingScreenButton->setIcon(Config::DownloadIcon());
            _ui.SettingsScreenButton->setIcon(Config::SettingsIcon());
            Animation::AnimatePosition(_ui.SideBar_LineIndicator, QPoint(0, _ui.ErrorScreenButton->y() - 2), 500);
            break;
        case Config::SETTINGS_SCREEN_INDEX:
            _ui.SettingsScreenButton->setIcon(Config::SettingsIconFilled());
            _ui.DownloadingScreenButton->setIcon(Config::DownloadIcon());
            if (_errorUI.count() > 0) _ui.ErrorScreenButton->setIcon(Config::ErrorIcon());
            Animation::AnimatePosition(_ui.SideBar_LineIndicator, QPoint(0, _ui.SettingsScreenButton->y() - 2), 500);
            break;
    }

    // Set Screen
    _ui.Screens->setCurrentIndex(screenIndex);
}

void SpotifyDownloader::ShowMessage(QString title, QString message, int msecs) {
    if (!Config::Notifications) return;
    _trayIcon->showMessage(title, message, QIcon(":/SpotifyDownloader/Icon.ico"), msecs);
}

void SpotifyDownloader::SetDownloadStatus(QString text) {
    _ui.DownloadedStatusLabel->setVisible(true);
    _ui.DownloadedStatusLabel->setText(text);
}

bool SpotifyDownloader::DownloaderUIExists(int threadIndex) {
    if (threadIndex < 0) return false;

    if (_downloaderUI.count() == 0) return false;

    // Sometimes a thread can be deleted before the request goes through
    if (_downloaderUI[threadIndex] == NULL) {
        return false;
    }

    return true;
}

void SpotifyDownloader::SetProgressLabel(int threadIndex, QString text) {
    if (!DownloaderUIExists(threadIndex)) return;

    _downloaderUI[threadIndex]->SetProgressText(text);
}

void SpotifyDownloader::SetProgressBar(int threadIndex, float percentage, int durationMs) {
    if (!DownloaderUIExists(threadIndex)) return;

    _downloaderUI[threadIndex]->SetProgressBar(percentage, durationMs);
}

void SpotifyDownloader::SetSongCount(int threadIndex, int currentCount, int totalCount) {
    if (threadIndex >= 0) {
        if (!DownloaderUIExists(threadIndex)) return;

        _downloaderUI[threadIndex]->SetSongCount(currentCount, totalCount);
        return;
    }

    _totalSongs = totalCount;
    _songsCompleted = currentCount;
    if (_songsCompleted < 0)
        _songsCompleted = 0;

    _ui.SongCount->setText(QString("%1/%2").arg(QString::number(_songsCompleted)).arg(QString::number(_totalSongs)));
    _ui.SongCount->adjustSize();
}

void SpotifyDownloader::SetSongImage(int threadIndex, QImage image) {
    if (!DownloaderUIExists(threadIndex)) return;

    _downloaderUI[threadIndex]->SetImage(image);
}

void SpotifyDownloader::SetSongDetails(int threadIndex, QString title, QString artists) {
    if (!DownloaderUIExists(threadIndex)) return;

    _downloaderUI[threadIndex]->SetTitle(title);
    _downloaderUI[threadIndex]->SetArtists(artists);
}

void SpotifyDownloader::SetErrorItems(QJsonArray tracks) {
    // Clear UI
    if (_errorUI.count() > 0) {
        foreach(SongErrorItem * errorItemUI, _errorUI) {
            if(errorItemUI != nullptr) delete errorItemUI;
        }
        _errorUI.clear();
    }

    foreach(QJsonValue val, tracks) {
        QJsonObject track = val.toObject();

        SongErrorItem* errorItem = new SongErrorItem();
        errorItem->Title->setText(track["title"].toString());
        errorItem->Album->setText(track["album"].toString());
        errorItem->Artists->setText(track["artists"].toString());
        errorItem->Image->setPixmap(JSONUtils::PixmapFromJSON(track["image"]).scaled(errorItem->Image->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

        _ui.ScrollLayout->insertWidget(_ui.ScrollLayout->count() - 1, errorItem);

        _errorUI.append(errorItem);
    }
}

void SpotifyDownloader::HidePauseWarning(int threadIndex) {
    if (!DownloaderUIExists(threadIndex)) return;

    _downloaderUI[threadIndex]->SetProgressText("Paused...");

    _threadsPaused++;
    if (_threadsPaused != _downloaderUI.count())
        return;

    _ui.PauseWarning->hide();
}

void SpotifyDownloader::UpdateBitrateInput(Codec::Extension codec) {
    // Get tooltip quality values
    int bitrateLowQuality = Codec::Data[codec].BitrateLowQuality;
    int bitrateGoodQuality = Codec::Data[codec].BitrateGoodQuality;
    int bitrateHighQuality = Codec::Data[codec].BitrateHighQuality;

    // Set tooltip
    QString tooltip = QString(
        R"(<html><head/><body><p><span style="font-weight: 700;">Recommended Values</span></p>
        <p><span style="font-weight: 700;">High Quality: </span>%1<br/>
        <span style="font-weight: 700;">Good Quality: </span>%2<br/>
        <span style="font-weight: 700;">Low Quality: </span>%3</p>
        <p>Only accepts numbers within 33-%4 with a multiple of 32 (excluding 33)</p></body></html>)"
    ).arg(bitrateHighQuality).arg(bitrateGoodQuality).arg(bitrateLowQuality).arg(bitrateHighQuality);

    _ui.AudioBitrateInput->setToolTip(tooltip);

    // Set maximum to highest quality
    _ui.AudioBitrateInput->setMaximum(bitrateHighQuality);
}

void SpotifyDownloader::SetThreadFinished(int threadIndex) {
    _ui.DownloadingSpaceFillerLayout->removeWidget(_downloaderUI[threadIndex]);
    DownloaderThread* downloaderThread = _downloaderUI[threadIndex];
    delete downloaderThread; 
    _downloaderUI[threadIndex] = NULL;
}