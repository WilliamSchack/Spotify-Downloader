#ifndef SPOTIFYDOWNLOADER_H
#define SPOTIFYDOWNLOADER_H

#include <QThread>

#include <QtWidgets/QDialog>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>

#include <QtCore/QFile>
#include <QFileDialog>
#include <QMessageBox>

#include <QSystemTrayIcon>
#include <QAction>
#include <QMenu>
#include <QStyle>

#include <QtUiTools/QUiLoader>

#include <QProcess>
#include <QFuture>
#include <QtConcurrent/QtConcurrentRun>

#include <QCloseEvent>

#include "ui_SpotifyDownloader.h"
#include "YTMusicAPI.h"
#include "SpotifyAPI.h"

// Forward Declarations
class PlaylistDownloader;
class SongDownloader;

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
        bool DownloadComplete = false;
        bool Overwrite = false;
        bool Notifications = true;

        int DownloadSpeed = 0;

        bool NormalizeAudio = true;
        float NormalizeAudioVolume = 0.0f;

        int TotalSongs;
        int CompletedSongs;

        bool Paused = false;
    public slots:
        void ChangeScreen(int screenIndex);
        void ShowMessage(QString title, QString message, int msecs = 5000);
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
        SongDownloader* _songDownloader;

        QSystemTrayIcon* _trayIcon;

        int _totalSongs = 0;
        int _songsCompleted = 0;

        void SetupDownloaderThread();

        void SetupTrayIcon();
        void SetupSetupScreen();
        void SetupSettingsScreen();
        void SetupProcessingScreen();

        void closeEvent(QCloseEvent* closeEvent);
};

class PlaylistDownloader : public QObject {
    Q_OBJECT

    public:
        ~PlaylistDownloader();
    public slots:
        void DownloadSongs(const SpotifyDownloader* main);

        void ChangeScreen(int screenIndex);
        void ShowMessage(QString title, QString message, int msecs = 5000);
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
        struct Worker {
            public:
                QThread Thread;
                SongDownloader* SongDownloader;
            signals:
                void operate(const PlaylistDownloader* main);
        };

        QList<Worker> _threads;
        void SetupThread();

        const QString CODEC = "mp3";
        const QString YTDLP_PATH = "yt-dlp.exe";
        const QString FFMPEG_PATH = "ffmpeg.exe";

        const SpotifyDownloader* Main;
        YTMusicAPI* _yt;
        SpotifyAPI* _sp;

        bool _quitting = false;

        QJsonArray _tracksNotFound;
        int _totalSongCount = 0;
    signals:
        void ChangeScreen(int screenIndex);
        void ShowMessage(QString title, QString message, int msecs = 5000);
        void SetProgressLabel(QString text);
        void SetProgressBar(float percentage);
        void SetSongCount(int currentCount, int totalCount);
        void SetSongImage(QPixmap image);
        void SetSongDetails(QString title, QString artists);
        void SetErrorItems(QJsonArray tracks);
        void HidePauseWarning();
};

class SongDownloader : public QObject {
    Q_OBJECT

    public:
        ~SongDownloader();

        static float Lerp(float a, float b, float t);
    public slots:
        void DownloadSongs(const SpotifyDownloader* main);
    private:
        void DownloadSong(QJsonObject track, int count, QJsonObject album);
        void CheckForStop();

        QString ValidateString(QString string);
        void DownloadImage(QString url, QString path, QSize resize = QSize());
        double LerpInList(std::vector<double> list, int index);

        const QString CODEC = "mp3";
        const QString YTDLP_PATH = "yt-dlp.exe";
        const QString FFMPEG_PATH = "ffmpeg.exe";

        const PlaylistDownloader* MainP;
        const SpotifyDownloader* Main;
        YTMusicAPI _yt;

        QProcess* _currentProcess;
        bool _quitting = false;

        QJsonArray _tracksNotFound;
        QJsonObject _currentTrack;
        int _totalSongCount = 0;
    signals:
        void ChangeScreen(int screenIndex);
        void ShowMessage(QString title, QString message, int msecs = 5000);
        void SetProgressLabel(QString text);
        void SetProgressBar(float percentage);
        void SetSongCount(int currentCount, int totalCount);
        void SetSongImage(QPixmap image);
        void SetSongDetails(QString title, QString artists);
        void SetErrorItems(QJsonArray tracks);
        void HidePauseWarning();
};

#endif