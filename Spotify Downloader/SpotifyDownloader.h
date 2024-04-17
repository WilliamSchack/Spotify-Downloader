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

        int ThreadCount = 3;
        int DownloadSpeed = 0;

        bool NormalizeAudio = true;
        float NormalizeAudioVolume = 0.0f;

        bool Paused = false;
    public slots:
        void SetupUI(int count);
        void ChangeScreen(int screenIndex);
        void ShowMessage(QString title, QString message, int msecs = 5000);
        void SetProgressLabel(int threadIndex, QString text);
        void SetProgressBar(int threadIndex, float percentage);
        void SetSongCount(int threadIndex, int currentCount, int totalCount);
        void SetSongImage(int threadIndex, QPixmap image);
        void SetSongDetails(int threadIndex, QString title, QString artists);
        void SetErrorItems(QJsonArray tracks);
        void HidePauseWarning(int threadIndex);
        void SetThreadFinished(int threadIndex);
    signals:
        void operate(const SpotifyDownloader* main);
        void RequestQuit();
    private:
        Ui::SpotifyDownloader _ui;
        PlaylistDownloader* _playlistDownloader;

        QSystemTrayIcon* _trayIcon;

        QList<DownloaderThread*> _downloaderUI;

        int _totalSongs = 0;
        int _songsCompleted = 0;

        int _threadsPaused = 0;

        void SetupDownloaderThread();

        void SetupTrayIcon();
        void SetupSetupScreen();
        void SetupSettingsScreen();
        void SetupProcessingScreen();

        void closeEvent(QCloseEvent* closeEvent);
};

class Worker {
    public:
        QThread Thread;
        SongDownloader* Downloader;
};

class PlaylistDownloader : public QObject {
    Q_OBJECT

    public:
        ~PlaylistDownloader();

        bool PauseNewDownloads = false;
    public slots:
        void DownloadSongs(const SpotifyDownloader* main);
        void SongDownloaded();
        void FinishThread(int threadIndex, QJsonArray tracksNotFound);
        void Quit();
        void CleanedUp();
    private:
        void SetupThreads(QList<QJsonArray> tracks, QJsonObject album);
        void DistributeTracks();
        void ClearDirFiles(const QString& path);

        QList<Worker*> _threads;

        const QString CODEC = "mp3";
        const QString YTDLP_PATH = "yt-dlp.exe";
        const QString FFMPEG_PATH = "ffmpeg.exe";

        const SpotifyDownloader* Main;
        YTMusicAPI* _yt;
        SpotifyAPI* _sp;

        bool _quitting = false;

        QJsonArray _tracksNotFound;
        int _totalSongCount = 0;

        int _threadCount = 0;
        int _songsDownloaded = 0;
        int _threadsFinished = 0;
        int _threadsCleaned = 0;
    signals:
        void SetupUI(int count);
        void DownloadOnThread(const SpotifyDownloader* main, const PlaylistDownloader* manager, YTMusicAPI* yt, QJsonArray tracks, QJsonObject album, int threadIndex);
        void QuitThreads();

        void ChangeScreen(int screenIndex);
        void ShowMessage(QString title, QString message, int msecs = 5000);
        void SetSongCount(int threadIndex, int currentCount, int totalCount);
        void SetErrorItems(QJsonArray tracks);
        void SetThreadFinished(int threadIndex);
};

class SongDownloader : public QObject {
    Q_OBJECT

    public:
        ~SongDownloader();

        int SongsDownloaded = 0;
        int TotalSongCount() const { return _totalSongCount; }

        void AddTracks(QJsonArray tracks);
        QJsonArray RemoveTracks(int numTracksToRemove);
        int SongsRemaining();
        void FinishedDownloading(bool finished);

        static float Lerp(float a, float b, float t);
    public slots:
        void DownloadSongs(const SpotifyDownloader* main, const PlaylistDownloader* manager, YTMusicAPI* yt, QJsonArray tracks, QJsonObject album, int threadIndex);
        void Quit();
    private:
        void StartDownload(int startIndex);
        void DownloadSong(QJsonObject track, int count, QJsonObject album);
        void CheckForStop();

        QString ValidateString(QString string);
        void DownloadImage(QString url, QString path, QSize resize = QSize());
        double LerpInList(std::vector<double> list, int index);

        const QString CODEC = "mp3";
        const QString YTDLP_PATH = "yt-dlp.exe";
        const QString FFMPEG_PATH = "ffmpeg.exe";

        const SpotifyDownloader* Main;
        const PlaylistDownloader* Manager;
        YTMusicAPI* _yt;

        int _threadIndex;

        bool _pausingNewDownloads = false;
        bool _waitingForFinishedResponse = false;
        bool _finishedDownloading = false;

        QProcess* _currentProcess;
        bool _quitting = false;

        QJsonObject _album;

        QJsonArray _downloadingTracks;
        QJsonArray _tracksNotFound;
        QJsonObject _currentTrack;
        int _totalSongCount = 0;
    signals:
        void ChangeScreen(int screenIndex);
        void ShowMessage(QString title, QString message, int msecs = 5000);
        void SetProgressLabel(int threadIndex, QString text);
        void SetProgressBar(int threadIndex, float percentage);
        void SetSongCount(int threadIndex, int currentCount, int totalCount);
        void SetSongImage(int threadIndex, QPixmap image);
        void SetSongDetails(int threadIndex, QString title, QString artists);
        void SetErrorItems(QJsonArray tracks);
        void HidePauseWarning(int threadIndex);
        void SongDownloaded();
        void Finish(int threadIndex, QJsonArray tracksNotFound);
        void RequestQuit();
        void CleanedUp();
};

#endif