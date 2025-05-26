#include "SpotifyDownloader.h"

#include <QMovie>

#include <qt_windows.h>

// Ui Setup
SpotifyDownloader::SpotifyDownloader(QWidget* parent) : QDialog(parent)
{
    qInfo() << "RUNNING VERSION" << VersionManager::VERSION;

    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);

    QCoreApplication::setOrganizationName(Config::ORGANIZATION_NAME);
    QCoreApplication::setApplicationName(Config::APPLICATION_NAME);

    qApp->installEventFilter(this);
    _ui.setupUi(this);

    _objectHoverWatcher = new ObjectHoverWatcher(this);

    SetupTrayIcon();
    SetupSideBar();
    SetupSetupScreen();
    SetupSettingsScreen();
    SetupProcessingScreen();

    Config::LoadSettings();
    LoadSettingsUI();

    SetupDownloaderThread();

    if (Config::CheckForUpdates)
        CheckForUpdate();

    qInfo() << "Successfully Initialised";
    Logger::Flush();
}

void SpotifyDownloader::SetupDownloaderThread() {
    // Get thread ready to be started
    _playlistDownloader = new PlaylistDownloader();
    _playlistDownloader->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, &workerThread, &QThread::quit);
    connect(&workerThread, &QThread::finished, _playlistDownloader, &PlaylistDownloader::deleteLater);

    connect(this, &SpotifyDownloader::operate, _playlistDownloader, &PlaylistDownloader::DownloadSongs);
    connect(this, &SpotifyDownloader::RequestQuit, _playlistDownloader, &PlaylistDownloader::Quit);
    connect(this, &SpotifyDownloader::DisplayFinalMessage, _playlistDownloader, &PlaylistDownloader::DisplayFinalMessage);

    // Allow thread to access ui elements
    connect(_playlistDownloader, &PlaylistDownloader::SetupUI, this, &SpotifyDownloader::SetupUI);
    connect(_playlistDownloader, &PlaylistDownloader::ChangeScreen, this, &SpotifyDownloader::ChangeScreen);
    connect(_playlistDownloader, &PlaylistDownloader::ShowMessage, this, &SpotifyDownloader::ShowMessage);
    connect(_playlistDownloader, &PlaylistDownloader::ShowMessageBox, this, &SpotifyDownloader::ShowMessageBox);
    connect(_playlistDownloader, &PlaylistDownloader::ShowMessageBoxWithButtons, this, &SpotifyDownloader::ShowMessageBoxWithButtons);
    connect(_playlistDownloader, &PlaylistDownloader::SetDownloadStatus, this, &SpotifyDownloader::SetDownloadStatus);
    connect(_playlistDownloader, &PlaylistDownloader::SetSongCount, this, &SpotifyDownloader::SetSongCount);
    connect(_playlistDownloader, &PlaylistDownloader::SetErrorItems, this, &SpotifyDownloader::SetErrorItems);
    connect(_playlistDownloader, &PlaylistDownloader::LoadSettingsUI, this, &SpotifyDownloader::LoadSettingsUI);
    connect(_playlistDownloader, &PlaylistDownloader::SetThreadFinished, this, &SpotifyDownloader::SetThreadFinished);
    connect(_playlistDownloader, &PlaylistDownloader::ResetDownloadingVariables, this, &SpotifyDownloader::ResetDownloadingVariables);
    connect(_playlistDownloader, &PlaylistDownloader::OpenURL, this, QOverload<QUrl>::of(& SpotifyDownloader::OpenURL));

    qInfo() << "Successfully Setup Playlist Worker Thread";
}

void SpotifyDownloader::ResetDownloadingVariables() {
    VariablesResetting = true;

    // Set Downloading Status
    if (_ui.DownloadedStatusLabel->text() == "") {
        if (_songsCompleted > 0) {
            int downloadErrors = _playlistDownloader->DownloadErrors();
            int tracksDownloaded = _songsCompleted - downloadErrors;
            SetDownloadStatus(QString("Successfully Downloaded %1 Song%2 With %3 Error%4")
                .arg(tracksDownloaded).arg(tracksDownloaded != 1 ? "s" : "")
                .arg(downloadErrors).arg(downloadErrors != 1 ? "s" : ""));
        } else {
            SetDownloadStatus("Downloading Complete");
        }

        _ui.DonationPromptButton->setVisible(true);
    }

    // Reset Variables
    DownloadStarted = false;

    _totalSongs = 0;
    _songsCompleted = 0;

    // Reset UI
    _ui.GettingPlaylistDataLabel->show();
    _ui.PlaylistURLInput->setText("");

    _ui.DownloaderThreadsInput->setEnabled(true);

    _ui.YoutubeCookiesClearButton->setEnabled(true);
    _ui.YoutubeCookiesPasteButton->setEnabled(true);

    _ui.POTokenInput->setEnabled(true);
    _ui.POTokenClearButton->setEnabled(true);
    _ui.POTokenPasteButton->setEnabled(true);

    _ui.SongCount->setText("0/0");
    _ui.SongCount->adjustSize();

    // Setup Threads
    SetupDownloaderThread();

    VariablesResetting = false;
}

QList<SpotifyDownloader::LineIndicator> SETTINGS_INDICATORS_CACHE;
QList<SpotifyDownloader::LineIndicator> SpotifyDownloader::SettingsLineIndicators() {
    // Return cache if already created
    if (!_settingsLineIndicatorsCache.isEmpty())
        return _settingsLineIndicatorsCache;

    // Calculate max heights
    int outputMaxHeight = 5 + 50 * (_ui.OutputSettingsScrollAreaWidgetContents->layout()->children().count() - 1);
    int downloadingMaxHeight = 5 + 50 * (_ui.DownloadingSettingsScrollAreaWidgetContents->layout()->children().count() - 1);
    int interfaceMaxHeight = 5 + 50 * (_ui.InterfaceSettingsScrollAreaWidgetContents->layout()->children().count() - 1);

    // Create indicator list
    QList<LineIndicator> lineIndicators {
        { _ui.OutputSettings_LineIndicator, outputMaxHeight, _ui.OutputSettingsScrollArea },
        { _ui.DownloadingSettings_LineIndicator, downloadingMaxHeight, _ui.DownloadingSettingsScrollArea },
        { _ui.InterfaceSettings_LineIndicator, interfaceMaxHeight, _ui.InterfaceSettingsScrollArea }
    };

    // Return indicators
    _settingsLineIndicatorsCache = lineIndicators;
    return lineIndicators;
}

void SpotifyDownloader::CheckForUpdate() {
    QMovie* updateRotatingGif = Config::UpdateRotatingIcon();
    _ui.UpdateImageLabel->setMovie(updateRotatingGif);
    updateRotatingGif->start();

    bool updateAvailable = VersionManager::UpdateAvailable();
    updateRotatingGif->stop();

    // Check connection to server
    if (VersionManager::LatestVersion().isEmpty()) {
        _ui.UpdateImageLabel->setPixmap(Config::UpdateIcon());
        return;
    }

    if (updateAvailable)
        _ui.UpdateImageLabel->setPixmap(Config::UpdateAvailableIcon());
    else
        _ui.UpdateImageLabel->setPixmap(Config::UpdateUpToDateIcon());

    qInfo() << "Latest Version:" << VersionManager::LatestVersion() << "| Update Available:" << updateAvailable;
}

void SpotifyDownloader::closeEvent(QCloseEvent* closeEvent) {
    if (DownloadStarted) {
        QMessageBox messageBox;
        messageBox.setWindowIcon(QIcon(":/SpotifyDownloader/Icon.ico"));
        messageBox.setWindowTitle("Are You Sure?");
        messageBox.setText(QString("Only %1 more to go!").arg(_totalSongs - _songsCompleted));
        messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int reply = messageBox.exec();

        if (reply == QMessageBox::Yes) {
            closeEvent->accept();
        }
        else closeEvent->ignore();
        return;
    }

    closeEvent->accept();
}

bool SpotifyDownloader::IsElevated() {
    BOOL fRet = false;
    HANDLE hToken = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION elevation;
        DWORD cbSize = sizeof(TOKEN_ELEVATION);
        if (GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &cbSize)) {
            fRet = elevation.TokenIsElevated;
        }
    }

    if (hToken) {
        CloseHandle(hToken);
    }

    return fRet;
}

// Application Exit
SpotifyDownloader::~SpotifyDownloader()
{
    qInfo() << "Quitting...";

    if (CurrentScreen() == Config::SETTINGS_SCREEN_INDEX)
        Config::SaveSettings();

    ExitingApplication = true;
    Paused = false;

    _trayIcon->hide();

    delete _objectHoverWatcher;

    emit RequestQuit();
    workerThread.wait();
}