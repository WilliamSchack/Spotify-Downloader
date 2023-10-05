#include "SpotifyDownloader.h"

void SpotifyDownloader::ChangeScreen(int screenIndex) {
    if (screenIndex == 2 || screenIndex == 3) DownloadComplete = true;
    _ui.Screens->setCurrentIndex(screenIndex);
}

void SpotifyDownloader::ShowMessage(QString title, QString message, int msecs) {
    if (!Notifications) return;
    _trayIcon->showMessage(title, message, QIcon(":/SpotifyDownloader/Icon.ico"), msecs);
}

void SpotifyDownloader::SetProgressLabel(QString text) {
    _ui.ProgressLabel->setText(text);
}

void SpotifyDownloader::SetProgressBar(float percentage) {
    _ui.ProgressBar_Front->resize(SongDownloader::Lerp(0, 334, percentage), 27);
}

void SpotifyDownloader::SetSongCount(int currentCount, int totalCount) {
    _songsCompleted = currentCount - 1;
    if (_songsCompleted < 0)
        _songsCompleted = 0;

    _totalSongs = totalCount;

    _ui.SongCount->setText(QString("%1/%2").arg(QString::number(currentCount)).arg(QString::number(totalCount)));
    _ui.SongCount->adjustSize();
}

void SpotifyDownloader::SetSongImage(QPixmap image) {
    image = image.scaled(_ui.SongImage->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    _ui.SongImage->setPixmap(image);
}

void SpotifyDownloader::SetSongDetails(QString title, QString artists) {
    _ui.SongTitle->setText(title);
    _ui.SongArtists->setText(artists);
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

void SpotifyDownloader::HidePauseWarning() {
    _ui.PauseWarning->hide();
    _ui.ProgressLabel->setText("Paused...");
}