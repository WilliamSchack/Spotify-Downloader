#include "SpotifyDownloader.h"

#include "Animation.h"
#include "MathUtils.h"

void SpotifyDownloader::SetupUI(int threadIndex) {
    _ui.GettingPlaylistDataLabel->hide();
    
    _downloaderUI.clear();
    for (int i = 0; i < threadIndex; i++) {
        DownloaderThread* currentUI = new DownloaderThread();
        _ui.DownloadingSpaceFillerLayout->insertWidget(_ui.DownloadingSpaceFillerLayout->count() - 1, currentUI);

        _downloaderUI.append(currentUI);
    }
}

int SpotifyDownloader::CurrentScreen() {
    return _ui.Screens->currentIndex();
}

void SpotifyDownloader::ChangeScreen(int screenIndex) {
    if (screenIndex == ERROR_SCREEN_INDEX) DownloadComplete = true;
    
    int currentScreen = CurrentScreen();

    // Animate Side Panel
    switch (screenIndex) {
        case SETUP_SCREEN_INDEX:
        case PROCESSING_SCREEN_INDEX:
            _ui.DownloadingScreenButton->setIcon(QIcon(":/SpotifyDownloader/Icons/Download_Icon_W_Filled.png"));
            if (_errorUI.count() > 0) _ui.ErrorScreenButton->setIcon(QIcon(":/SpotifyDownloader/Icons/Error_Icon_W.png"));
            _ui.SettingsScreenButton->setIcon(QIcon(":/SpotifyDownloader/Icons/SettingsCog_W.png"));
            Animation::AnimatePosition(_ui.SideBar_LineIndicator, QPoint(0, _ui.DownloadingScreenButton->y() - 2), 500);
            break;
        case ERROR_SCREEN_INDEX:
            _ui.ErrorScreenButton->setIcon(QIcon(":/SpotifyDownloader/Icons/Error_Icon_W_Filled.png"));
            _ui.DownloadingScreenButton->setIcon(QIcon(":/SpotifyDownloader/Icons/Download_Icon_W.png"));
            _ui.SettingsScreenButton->setIcon(QIcon(":/SpotifyDownloader/Icons/SettingsCog_W.png"));
            Animation::AnimatePosition(_ui.SideBar_LineIndicator, QPoint(0, _ui.ErrorScreenButton->y() - 2), 500);
            break;
        case SETTINGS_SCREEN_INDEX:
            _ui.SettingsScreenButton->setIcon(QIcon(":/SpotifyDownloader/Icons/SettingsCog_W_Filled.png"));
            _ui.DownloadingScreenButton->setIcon(QIcon(":/SpotifyDownloader/Icons/Download_Icon_W.png"));
            if (_errorUI.count() > 0) _ui.ErrorScreenButton->setIcon(QIcon(":/SpotifyDownloader/Icons/Error_Icon_W.png"));
            Animation::AnimatePosition(_ui.SideBar_LineIndicator, QPoint(0, _ui.SettingsScreenButton->y() - 2), 500);
            break;
    }

    // Set Screen
    _ui.Screens->setCurrentIndex(screenIndex);
}

void SpotifyDownloader::ShowMessage(QString title, QString message, int msecs) {
    if (!Notifications) return;
    _trayIcon->showMessage(title, message, QIcon(":/SpotifyDownloader/Icon.ico"), msecs);
}

void SpotifyDownloader::SetDownloadStatus(QString text) {
    _ui.DownloadedStatusLabel->setText(text);
}

void SpotifyDownloader::SetProgressLabel(int threadIndex, QString text) {
    if (_downloaderUI.count() == 0) return;

    _downloaderUI[threadIndex]->ProgressLabel->setText(text);
}

void SpotifyDownloader::SetProgressBar(int threadIndex, float percentage) {
    if (_downloaderUI.count() == 0) return;

    _downloaderUI[threadIndex]->ProgressBar_Front->resize(MathUtils::Lerp(0, 373, percentage), 27);
}

void SpotifyDownloader::SetSongCount(int threadIndex, int currentCount, int totalCount) {
    if (threadIndex >= 0) {
        if (threadIndex > _downloaderUI.count()) return; // Sometimes a thread can be deleted before this request goes through

        QLabel* songCount = _downloaderUI[threadIndex]->SongCount;
        songCount->setText(QString("%1/%2").arg(QString::number(currentCount)).arg(QString::number(totalCount)));
        songCount->adjustSize();
        return;
    }

    _totalSongs = totalCount;
    _songsCompleted = currentCount;
    if (_songsCompleted < 0)
        _songsCompleted = 0;

    _ui.SongCount->setText(QString("%1/%2").arg(QString::number(_songsCompleted)).arg(QString::number(_totalSongs)));
    _ui.SongCount->adjustSize();
}

void SpotifyDownloader::SetSongImage(int threadIndex, QPixmap image) {
    if (_downloaderUI.count() == 0) return;

    image = image.scaled(_downloaderUI[threadIndex]->SongImage->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    _downloaderUI[threadIndex]->SongImage->setPixmap(image);
}

void SpotifyDownloader::SetSongDetails(int threadIndex, QString title, QString artists) {
    if (_downloaderUI.count() == 0) return;

    _downloaderUI[threadIndex]->SongTitle->setText(title);
    _downloaderUI[threadIndex]->SongArtists->setText(artists);
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
    if (_downloaderUI.count() == 0) return;

    _downloaderUI[threadIndex]->ProgressLabel->setText("Paused...");

    _threadsPaused++;
    if (_threadsPaused != _downloaderUI.count())
        return;

    _ui.PauseWarning->hide();
}

void SpotifyDownloader::SetThreadFinished(int threadIndex) {
    _ui.DownloadingSpaceFillerLayout->removeWidget(_downloaderUI[threadIndex]);
    delete _downloaderUI[threadIndex];
}