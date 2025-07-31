#ifndef SPOTIFYDOWNLOADER_H
#define SPOTIFYDOWNLOADER_H

#define QT_MESSAGELOGCONTEXT
#include "Utilities/Logger.h"

#include <QThread>

#include "ui_SpotifyDownloader.h"

#include "Config.h"

#include "UI/CustomWidgets/DownloaderThread.h"
#include "UI/CustomWidgets/SongErrorItem.h"

#include "Utilities/ObjectHoverWatcher.h"

#include "Network/YTMusicAPI.h"
#include "Network/SpotifyAPI.h"
#include "Network/VersionManager.h"

#include "Downloading/Codec.h"

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

// Forward Declarations
class PlaylistDownloader;
class SongDownloader;

class SpotifyDownloader : public QDialog
{
    Q_OBJECT
    QThread workerThread;

    public:
        struct LineIndicator {
            QWidget* Indicator;
            int MaxHeight = 0; // Hidden widgets modify this height themselves

            QScrollArea* ScrollArea = nullptr;
        };

        bool ExitingApplication = false;
        bool Paused = false;

        bool DownloadStarted = false;

        bool VariablesResetting = false;
    public:
        SpotifyDownloader(QWidget* parent = nullptr);
        ~SpotifyDownloader();

        virtual bool eventFilter(QObject* obj, QEvent* event) Q_DECL_OVERRIDE;
    public slots:
        void SetupUI(int count);
        void ChangeScreen(int screenIndex);
        void ShowMessage(QString title, QString message, int msecs = 5000);
        void ShowMessageBox(QString title, QString message, QMessageBox::Icon icon);
        int ShowMessageBoxWithButtons(QString title, QString message, QMessageBox::Icon icon, QMessageBox::StandardButtons standardButtons);
        void SetDownloadStatus(QString text);
        void SetProgressLabel(int threadIndex, QString text);
        void SetProgressBar(int threadIndex, float percentage, int durationMs = 1000);
        void SetSongCount(int threadIndex, int currentCount, int totalCount);
        void SetSongImage(int threadIndex, QImage image);
        void SetSongDetails(int threadIndex, QString title, QString artists);
        void SetErrorItems(QJsonArray tracks);
        void LoadSettingsUI();
        void HidePauseWarning(int threadIndex);
        void SetThreadFinished(int threadIndex);
        void OpenURL(QUrl address);
    signals:
        void operate(const SpotifyDownloader* main);
        void RequestQuit();
        void DisplayFinalMessage();
    private:
        QList<LineIndicator> SettingsLineIndicators();
        QList<LineIndicator> _settingsLineIndicatorsCache;

        Ui::SpotifyDownloader _ui;
        PlaylistDownloader* _playlistDownloader;

        QSystemTrayIcon* _trayIcon;

        QList<DownloaderThread*> _downloaderUI;
        QList<SongErrorItem*> _errorUI;

        ObjectHoverWatcher* _objectHoverWatcher;

        int _totalSongs = 0;
        int _songsCompleted = 0;

        int _threadsPaused = 0;
    private:
        void SetupDownloaderThread();
        bool DownloaderUIExists(int threadIndex);

        void SetupTrayIcon();
        void SetupSideBar();
        void SetupSetupScreen();
        void SetupSettingsScreen();
        void SetupProcessingScreen();

        void UpdateBitrateInput(Codec::Extension codec);

        bool ValidateSettings();
        bool ValidateInputs();
        bool ValidateURL();
        bool ValidateDirectory();

        void ResetDownloadingVariables();

        void CheckForUpdate();

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
        bool PauseNewDownloads = false;
    public:
        ~PlaylistDownloader();

        int DownloadErrors();
    public slots:
        void DownloadSongs(const SpotifyDownloader* main);
        void SongDownloaded(QString filePath);
        void ShowPOTokenError();
        void AddDownloadErrors(int threadIndex, QJsonArray downloadErrors);
        void FinishThread(int threadIndex, QJsonArray dowwnloadErrors);
        void Quit();
        void CleanedUp(int threadIndex);
        void DisplayFinalMessage();
    private:
        QList<Worker*> _threads;

        const SpotifyDownloader* Main;
        YTMusicAPI* _yt;
        SpotifyAPI* _sp;

        bool _earlyQuit = false;
        bool _quitting = false;
        bool _poTokenErrorShown = false;

        QString _playlistName = "";
        QString _playlistOwner = "";

        QJsonArray _downloadErrors;
        int _totalSongCount = 0;

        int _threadCount = 0;
        int _songsDownloaded = 0;
        int _threadsFinished = 0;
        int _threadsCleaned = 0;

        QStringList _downloadedSongFilePaths;
    private:
        void SetupThreads(QList<QJsonArray> tracks, QJsonObject album);
        bool DistributeTracks();
        void ClearDirFiles(const QString& path);

        std::tuple<QString, bool> PlaylistFileNameTagHandler(QString tag);
    signals:
        void SetupUI(int count);
        void DownloadOnThread(const SpotifyDownloader* main, const PlaylistDownloader* manager, YTMusicAPI* yt, QJsonArray tracks, QJsonObject album, int threadIndex);
        void QuitThreads();

        void ChangeScreen(int screenIndex);
        void ShowMessage(QString title, QString message, int msecs = 5000);
        void ShowMessageBox(QString title, QString message, QMessageBox::Icon icon);
        void ShowMessageBoxWithButtons(QString title, QString message, QMessageBox::Icon icon, QMessageBox::StandardButtons standardButtons);
        void SetDownloadStatus(QString text);
        void SetSongCount(int threadIndex, int currentCount, int totalCount);
        void SetErrorItems(QJsonArray tracks);
        void LoadSettingsUI();
        void SetThreadFinished(int threadIndex);
        void ResetDownloadingVariables();
        void OpenURL(QUrl address);
};

#include "Downloading/Song.h"
class Song;

class SongDownloader : public QObject {
    Q_OBJECT

    public:
        int SongsDownloaded = 0;

        int TotalSongCount() const { return _totalSongCount; }
    public:
        ~SongDownloader();

        void AddTracks(QJsonArray tracks);
        QJsonArray RemoveTracks(int numTracksToRemove);
        int SongsRemaining();
        void FinishedDownloading(bool finished);
    public slots:
        void DownloadSongs(const SpotifyDownloader* main, const PlaylistDownloader* manager, YTMusicAPI* yt, QJsonArray tracks, QJsonObject album, int threadIndex);
        void Quit();
    private:
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
        QJsonArray _downloadErrors;
        QJsonObject _currentTrack;
        int _totalSongCount = 0;
    private:
        void StartDownload(int startIndex);
        QString DownloadSong(QJsonObject track, int count, QJsonObject album);
        void AddSongToErrors(Song song, QString error, QString searchQuery = "", bool silent = false);
        void CheckForStop();
    signals:
        void ChangeScreen(int screenIndex);
        void ShowMessage(QString title, QString message, int msecs = 5000);
        void ShowMessageBox(QString title, QString message, QMessageBox::Icon icon);
        void ShowMessageBoxWithButtons(QString title, QString message, QMessageBox::Icon icon, QMessageBox::StandardButtons standardButtons);
        void ShowPOTokenError();
        void SetDownloadStatus(QString text);
        void SetProgressLabel(int threadIndex, QString text);
        void SetProgressBar(int threadIndex, float percentage, int durationMs = 1000);
        void SetSongCount(int threadIndex, int currentCount, int totalCount);
        void SetSongImage(int threadIndex, QImage image);
        void SetSongDetails(int threadIndex, QString title, QString artists);
        void SetErrorItems(QJsonArray tracks);
        void LoadSettingsUI();
        void HidePauseWarning(int threadIndex);
        void SongDownloaded(QString filePath);
        void AddDownloadErrors(int threadIndex, QJsonArray downloadErrors);
        void Finish(int threadIndex, QJsonArray downloadErrors);
        void RequestQuit();
        void CleanedUp(int threadIndex);
};

#endif