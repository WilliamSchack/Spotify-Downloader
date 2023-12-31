#include "SpotifyDownloader.h"

void SpotifyDownloader::SetupUI(int threadIndex) {
    _downloaderUI.clear();

    for (int i = 0; i < threadIndex; i++) {
        DownloaderThread* currentUI = new DownloaderThread();
        _ui.DownloadingSpaceFillerLayout->insertWidget(_ui.DownloadingSpaceFillerLayout->count() - 1, currentUI);

        _downloaderUI.append(currentUI);
    }
}

void SpotifyDownloader::ChangeScreen(int screenIndex) {
    if (screenIndex == 2 || screenIndex == 3) DownloadComplete = true;
    _ui.Screens->setCurrentIndex(screenIndex);
}

void SpotifyDownloader::ShowMessage(QString title, QString message, int msecs) {
    if (!Notifications) return;
    _trayIcon->showMessage(title, message, QIcon(":/SpotifyDownloader/Icon.ico"), msecs);
}

void SpotifyDownloader::SetProgressLabel(int threadIndex, QString text) {
    _downloaderUI[threadIndex]->ProgressLabel->setText(text);
}

void SpotifyDownloader::SetProgressBar(int threadIndex, float percentage) {
    _downloaderUI[threadIndex]->ProgressBar_Front->resize(SongDownloader::Lerp(0, 427, percentage), 27);
}

void SpotifyDownloader::SetSongCount(int threadIndex, int currentCount, int totalCount) {
    if (threadIndex >= 0) {
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
    image = image.scaled(_downloaderUI[threadIndex]->SongImage->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    _downloaderUI[threadIndex]->SongImage->setPixmap(image);
}

void SpotifyDownloader::SetSongDetails(int threadIndex, QString title, QString artists) {
    _downloaderUI[threadIndex]->SongTitle->setText(title);
    _downloaderUI[threadIndex]->SongArtists->setText(artists);
}

void SpotifyDownloader::SetErrorItems(QJsonArray tracks) {
    foreach(QJsonValue val, tracks) {
        QJsonObject track = val.toObject();

        SongErrorItem* errorItem = new SongErrorItem();
        errorItem->Title->setText(track["title"].toString());
        errorItem->Album->setText(track["album"].toString());
        errorItem->Artists->setText(track["artists"].toString());
        errorItem->Image->setPixmap(JSONUtils::PixmapFromJSON(track["image"]).scaled(errorItem->Image->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

        _ui.ScrollLayout->insertWidget(_ui.ScrollLayout->count() - 1, errorItem);
    }
}

void SpotifyDownloader::HidePauseWarning(int threadIndex) {
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