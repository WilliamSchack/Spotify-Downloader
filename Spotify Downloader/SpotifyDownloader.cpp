#include "SpotifyDownloader.h"

#include <QDesktopServices>

#include <qt_windows.h>

// Ui Setup
SpotifyDownloader::SpotifyDownloader(QWidget* parent) : QDialog(parent)
{
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
    connect(_playlistDownloader, &PlaylistDownloader::SetDownloadStatus, this, &SpotifyDownloader::SetDownloadStatus);
    connect(_playlistDownloader, &PlaylistDownloader::SetSongCount, this, &SpotifyDownloader::SetSongCount);
    connect(_playlistDownloader, &PlaylistDownloader::SetErrorItems, this, &SpotifyDownloader::SetErrorItems);
    connect(_playlistDownloader, &PlaylistDownloader::SetThreadFinished, this, &SpotifyDownloader::SetThreadFinished);
    connect(_playlistDownloader, &PlaylistDownloader::ResetDownloadingVariables, this, &SpotifyDownloader::ResetDownloadingVariables);

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
    _ui.SongCount->setText("0/0");
    _ui.SongCount->adjustSize();

    // Setup Threads
    SetupDownloaderThread();

    VariablesResetting = false;
}

QList<SpotifyDownloader::LineIndicator> SETTINGS_INDICATORS_CACHE;
QList<SpotifyDownloader::LineIndicator> SpotifyDownloader::SETTINGS_LINE_INDICATORS() {
    if (!SETTINGS_LINE_INDICATORS_CACHE.isEmpty())
        return SETTINGS_LINE_INDICATORS_CACHE;

    QList<LineIndicator> lineIndicators {
        { _ui.OutputSettings_LineIndicator, Config::OUTPUT_SETTINGS_LINE_MAX_HEIGHT, _ui.OutputSettingsScrollArea },
        { _ui.DownloadingSettings_LineIndicator, Config::DOWNLOADING_SETTINGS_LINE_MAX_HEIGHT, nullptr },
        { _ui.InterfaceSettings_LineIndicator, Config::INTERFACE_SETTINGS_LINE_MAX_HEIGHT, nullptr }
    };

    SETTINGS_LINE_INDICATORS_CACHE = lineIndicators;
    return lineIndicators;
}

void SpotifyDownloader::OpenURL(QUrl address, QString windowTitle, QString windowMessage) {

    QMessageBox msg = QMessageBox();
    msg.setWindowTitle(windowTitle);
    msg.setText(QString("%1\n\nThis will open in your browser").arg(windowMessage));
    msg.setIcon(QMessageBox::Question);
    msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    int reply = msg.exec();

    if(reply == QMessageBox::Yes)
        QDesktopServices::openUrl(address);
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