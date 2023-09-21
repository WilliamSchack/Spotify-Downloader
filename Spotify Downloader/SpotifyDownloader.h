#ifndef SPOTIFYDOWNLOADER_H
#define SPOTIFYDOWNLOADER_H

#include <QThread>

#include <QtWidgets/QDialog>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>

#include <QtCore/QFile>

#include <QtUiTools/QUiLoader>

#include <QFileDialog>

#include <QMessageBox>

#include <QFuture>
#include <QtConcurrent/QtConcurrentRun>

#include <QProcess>

#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/id3v2frame.h>
#include <taglib/commentsframe.h>
#include <taglib/attachedpictureframe.h>
#include <taglib/textidentificationframe.h>

#include "ui_SpotifyDownloader.h"
#include "CustomWidgets.h"

#include "SpotifyAPI.h"
#include "YTMusicAPI.h"
#include "Network.h"
#include "difflib.h"

class SpotifyDownloader : public QDialog
{
    Q_OBJECT
    QThread workerThread;

    public:
        SpotifyDownloader(QWidget *parent = nullptr);
        ~SpotifyDownloader();

        QString PlaylistURLText;
        QString SaveLocationText;

        bool DownloadStarted = false;
        bool Overwrite = false;
        bool Notifications = true;

        int DownloadSpeed = 0;

        bool NormalizeAudio = true;
        float NormalizeAudioVolume = 0.0f;

        bool Paused = false;
    public slots:
        void ChangeScreen(int screenIndex);
        void SetProgressLabel(QString text);
        void SetProgressBar(float percentage);
        void SetSongCount(int currentCount, int totalCount);
        void SetSongImage(QPixmap image);
        void SetSongDetails(QString title, QString artists);
        void SetErrorItems(QJsonArray tracks);
        void HidePauseWarning();
    signals:
        void operate(const SpotifyDownloader* main);
    private:
        Ui::SpotifyDownloader _ui;

        void SetupSetupScreen();
        void SetupSettingsScreen();
        void SetupProcessingScreen();
};

class SongDownloader : public QObject {
    Q_OBJECT

    public:
        static float Lerp(float a, float b, float t);
    public slots:
        void DownloadSongs(const SpotifyDownloader* maiin);
    private:
        void DownloadSong(QJsonObject track, int count, QJsonObject album);
        void CheckIfPaused();

        const SpotifyDownloader* Main;

        QString ValidateString(QString string);
        void DownloadImage(QString url, QString path, QSize resize = QSize());
        double LerpInList(std::vector<double> list, int index);

        const QString CODEC = "mp3";
        const QString YTDLP_PATH = "yt-dlp.exe";
        const QString FFMPEG_PATH = "ffmpeg.exe";

        YTMusicAPI _yt;

        QJsonArray _tracksNotFound;

        int _totalSongCount = 0;
    signals:
        void ChangeScreen(int screenIndex);
        void SetProgressLabel(QString text);
        void SetProgressBar(float percentage);
        void SetSongCount(int currentCount, int totalCount);
        void SetSongImage(QPixmap image);
        void SetSongDetails(QString title, QString artists);
        void SetErrorItems(QJsonArray tracks);
        void HidePauseWarning();
};

#endif