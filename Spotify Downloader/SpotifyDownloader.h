#ifndef SPOTIFYDOWNLOADER_H
#define SPOTIFYDOWNLOADER_H

#define QT_MESSAGELOGCONTEXT
#include "Logger.h"

#include <QThread>

#include <QSettings>

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

#include <QEvent>
#include <QCloseEvent>

#include "ui_SpotifyDownloader.h"
#include "ObjectHoverWatcher.h"

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

        static constexpr const char* VERSION = "1.4.2-testing";

        static const int SETUP_SCREEN_INDEX = 0;
        static const int SETTINGS_SCREEN_INDEX = 1;
        static const int PROCESSING_SCREEN_INDEX = 2;
        static const int ERROR_SCREEN_INDEX = 3;

        static constexpr const char* NAMING_TAGS[] = {
            "song name",
            "album name",
            "song artist",
            "song artists",
            "album artist",
            "album artists",
            "song time seconds",
            "song time sinutes",
            "song time hours"
        };

        enum class NamingError {
            None,
            EnclosingTagsInvalid,
            TagInvalid
        };

        QString PlaylistURLText;
        QString SaveLocationText;

        bool ExitingApplication = false;
        bool Paused = false;

        bool DownloadStarted = false;
        bool DownloadComplete = false;

        bool VariablesResetting = false;

        // Below variables will be loaded in LoadSettings()
        bool Overwrite = false;
        bool Notifications = true;

        int ThreadCount = 3;
        float DownloadSpeed = 0;

        bool NormalizeAudio = true;
        float NormalizeAudioVolume = -14.0;

        int AudioBitrate = 192;

        QString SongOutputFormatTag = "<>";
        QString SongOutputFormat = "<Song Name> - <Song Artist>";

        int FolderSortingIndex = 0;

        static QStringList Q_NAMING_TAGS();
        std::tuple<QString, NamingError> FormatOutputNameWithTags(std::function<QString(QString)> tagHandlerFunc) const; // Output, Error

        virtual bool eventFilter(QObject* obj, QEvent* event) Q_DECL_OVERRIDE;
    public slots:
        void SetupUI(int count);
        void ChangeScreen(int screenIndex);
        void ShowMessage(QString title, QString message, int msecs = 5000);
        void SetDownloadStatus(QString text);
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
        void DisplayFinalMessage();
    private:
        const QString ORGANIZATION_NAME = "WilliamSchack";
        const QString APPLICATION_NAME = "Spotify Downloader";

        const int OUTPUT_SETTINGS_LINE_MAX_HEIGHT = 255;
        const int DOWNLOADING_SETTINGS_LINE_MAX_HEIGHT = 105;

        static QStringList Q_NAMING_TAGS_CACHE;

        Ui::SpotifyDownloader _ui;
        PlaylistDownloader* _playlistDownloader;

        QSystemTrayIcon* _trayIcon;

        QList<DownloaderThread*> _downloaderUI;
        QList<SongErrorItem*> _errorUI;

        ObjectHoverWatcher* _objectHoverWatcher;

        int _totalSongs = 0;
        int _songsCompleted = 0;

        int _threadsPaused = 0;

        void SetupDownloaderThread();

        void SetupTrayIcon();
        void SetupSideBar();
        void SetupSetupScreen();
        void SetupSettingsScreen();
        void SetupProcessingScreen();

        bool ValidateSettings();
        bool ValidateInputs();
        bool ValidateURL();
        bool ValidateDirectory();

        void SaveSettings();
        void LoadSettings();

        void ResetDownloadingVariables();

        int CurrentScreen();

        void OpenURL(QUrl address, QString windowTitle, QString windowMessage);

        void closeEvent(QCloseEvent* closeEvent);

        bool IsElevated();
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

        int TracksNotFound();
    public slots:
        void DownloadSongs(const SpotifyDownloader* main);
        void SongDownloaded();
        void FinishThread(int threadIndex, QJsonArray tracksNotFound);
        void Quit();
        void CleanedUp(int threadIndex);
        void DisplayFinalMessage();
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
        void SetDownloadStatus(QString text);
        void SetSongCount(int threadIndex, int currentCount, int totalCount);
        void SetErrorItems(QJsonArray tracks);
        void SetThreadFinished(int threadIndex);
        void ResetDownloadingVariables();
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
    public slots:
        void DownloadSongs(const SpotifyDownloader* main, const PlaylistDownloader* manager, YTMusicAPI* yt, QJsonArray tracks, QJsonObject album, int threadIndex);
        void Quit();
    private:
        void StartDownload(int startIndex);
        void DownloadSong(QJsonObject track, int count, QJsonObject album);
        void CheckForStop();

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
        void CleanedUp(int threadIndex);
};

#endif