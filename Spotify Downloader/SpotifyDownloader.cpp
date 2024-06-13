#include "SpotifyDownloader.h"

#include "CustomWidgets.h"

#include <QDesktopServices>

#include <qt_windows.h>

// Ui Setup
SpotifyDownloader::SpotifyDownloader(QWidget* parent) : QDialog(parent)
{
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);

    QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
    QCoreApplication::setApplicationName(APPLICATION_NAME);

    _ui.setupUi(this);

    _buttonHoverWatcher = new QButtonHoverWatcher(this);

    SetupTrayIcon();
    SetupSideBar();
    SetupSetupScreen();
    SetupSettingsScreen();
    SetupProcessingScreen();

    LoadSettings();

    SetupDownloaderThread();
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
}

void SpotifyDownloader::SaveSettings() {
    // Get values from UI
    bool overwriteEnabled = _ui.OverwriteSettingButton->isChecked;
    bool normalizeEnabled = _ui.NormalizeVolumeSettingButton->isChecked;
    float normalizeVolume = _ui.NormalizeVolumeSettingInput->value();
    int audioBitrate = _ui.AudioBitrateInput->value();
    QString songOutputFormatTag = _ui.SongOutputFormatTagInput->text();
    QString songOutputFormat = _ui.SongOutputFormatInput->text();
    bool statusNotificationsEnabled = _ui.NotificationSettingButton->isChecked;
    int downloaderThreads = _ui.DownloaderThreadsInput->value();
    float downloadSpeedLimit = _ui.DownloadSpeedSettingInput->value();

    // Save values to settings
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);

    settings.beginGroup("Output");
    settings.setValue("overwriteEnabled", overwriteEnabled);
    settings.setValue("normalizeEnabled", normalizeEnabled);
    settings.setValue("normalizeVolume", normalizeVolume);
    settings.setValue("audioBitrate", audioBitrate);
    settings.setValue("songOutputFormatTag", songOutputFormatTag);
    settings.setValue("songOutputFormat", songOutputFormat);
    settings.endGroup();

    settings.beginGroup("Downloading");
    settings.setValue("statusNotificationsEnabled", statusNotificationsEnabled);
    settings.setValue("downloaderThreads", downloaderThreads);
    settings.setValue("downloadSpeedLimit", downloadSpeedLimit);
    settings.endGroup();
}

void SpotifyDownloader::LoadSettings() {
    // Clicking buttons to call their callbacks
    // Default settings are defined here

    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);

    settings.beginGroup("Output");

    // Overwrite
    bool overwriteEnabled = settings.value("overwriteEnabled", false).toBool();
    if(_ui.OverwriteSettingButton->isChecked != overwriteEnabled)
        _ui.OverwriteSettingButton->click();
    Overwrite = overwriteEnabled;

    // Normalize Volume
    bool normalizeEnabled = settings.value("normalizeEnabled", true).toBool();
    if (_ui.NormalizeVolumeSettingButton->isChecked != normalizeEnabled)
        _ui.NormalizeVolumeSettingButton->click();
    NormalizeAudio = normalizeEnabled;

    float normalizeVolume = settings.value("normalizeVolume", 14.0).toFloat();
    _ui.NormalizeVolumeSettingInput->setValue(normalizeVolume);
    NormalizeAudioVolume = normalizeVolume;

    // Audio Bitrate
    int audioBitrate = settings.value("audioBitrate", 192).toInt();
    _ui.AudioBitrateInput->setValue(audioBitrate);
    AudioBitrate = audioBitrate;

    float estimatedFileSize = (((float)AudioBitrate * 60) / 8) / 1024;
    QString fileSizeText = QString("%1MB/min").arg(QString::number(estimatedFileSize, 'f', 2));
    _ui.AudioBitrateFileSizeLabel_Value->setText(fileSizeText);

    // Save Location
    QString saveLocation = settings.value("saveLocation", "").toString();
    _ui.SaveLocationInput->setText(saveLocation);

    // Song Output Format
    QString songOutputFormatTag = settings.value("songOutputFormatTag", "<>").toString();
    _ui.SongOutputFormatTagInput->setText(songOutputFormatTag);
    SongOutputFormatTag = songOutputFormatTag;

    QString songOutputFormat = settings.value("songOutputFormat", "<Song Name> - <Song Artist>").toString();
    _ui.SongOutputFormatInput->setText(songOutputFormat);
    SongOutputFormat = songOutputFormat;

    settings.endGroup();

    settings.beginGroup("Downloading");

    // Status Notifications
    bool statusNotificationsEnabled = settings.value("statusNotificationsEnabled", true).toBool();
    if (_ui.NotificationSettingButton->isChecked != statusNotificationsEnabled)
        _ui.NotificationSettingButton->click();
    Notifications = statusNotificationsEnabled;

    // Downloader Threads
    int downloaderThreads = settings.value("downloaderThreads", 3).toInt();
    _ui.DownloaderThreadsInput->setValue(downloaderThreads);
    ThreadCount = downloaderThreads;

    // Download Speed Limit
    float downloadSpeedLimit = settings.value("downloadSpeedLimit", 0.0).toFloat();
    _ui.DownloadSpeedSettingInput->setValue(downloadSpeedLimit);
    DownloadSpeed = downloadSpeedLimit;

    settings.endGroup();
}

void SpotifyDownloader::ResetDownloadingVariables() {
    VariablesResetting = true;

    // Set Downloading Status
    if (_ui.DownloadedStatusLabel->text() == "") {
        if (_songsCompleted > 0) {
            int tracksNotFound = _playlistDownloader->TracksNotFound();
            int tracksDownloaded = _songsCompleted - tracksNotFound;
            SetDownloadStatus(QString("Successfully Downloaded %1 Song%2 With %3 Error%4")
                .arg(_songsCompleted - tracksNotFound).arg(tracksDownloaded != 1 ? "s" : "")
                .arg(tracksNotFound).arg(tracksNotFound != 1 ? "s" : ""));
        } else {
            SetDownloadStatus("Downloading Complete");
        }
    }

    // Reset Variables
    DownloadStarted = false;
    DownloadComplete = false;

    _totalSongs = 0;
    _songsCompleted = 0;

    // Reset UI
    _ui.PlaylistURLInput->setText("");
    _ui.DownloaderThreadsInput->setEnabled(true);
    _ui.SongCount->setText("0/0");
    _ui.SongCount->adjustSize();

    // Setup Threads
    SetupDownloaderThread();

    VariablesResetting = false;
}

// Convert naming tags to QStringList, cannot have const QStringList
QStringList SpotifyDownloader::Q_NAMING_TAGS_CACHE;
QStringList SpotifyDownloader::Q_NAMING_TAGS() {
    if (!Q_NAMING_TAGS_CACHE.isEmpty())
        return Q_NAMING_TAGS_CACHE;

    QStringList tags = QStringList();
    int numTags = std::extent<decltype(NAMING_TAGS)>::value;
    for (int i = 0; i < numTags; i++) {
        QString tag = QString::fromStdString(NAMING_TAGS[i]);
        tags.append(tag);
    }

    Q_NAMING_TAGS_CACHE = tags;
    return tags;
}

std::tuple<QString, SpotifyDownloader::NamingError> SpotifyDownloader::FormatOutputNameWithTags(std::function<QString(QString)> tagHandlerFunc) const {
    QString songOutputFormatTag = SongOutputFormatTag;
    QString songOutputFormat = SongOutputFormat;

    if (songOutputFormatTag.length() != 2) {
        return std::make_tuple(songOutputFormatTag, NamingError::EnclosingTagsInvalid);
    }

    QChar leftTag = songOutputFormatTag[0];
    QChar rightTag = songOutputFormatTag[1];

    QStringList namingTags = Q_NAMING_TAGS();

    QString newString;
    int currentCharIndex = 0;
    while (currentCharIndex <= songOutputFormat.length()) {
        int nextLeftIndex = songOutputFormat.indexOf(leftTag, currentCharIndex);
        int nextRightIndex = songOutputFormat.indexOf(rightTag, currentCharIndex);
        int tagLength = nextRightIndex - nextLeftIndex - 1;
        QString tag = songOutputFormat.mid(nextLeftIndex + 1, tagLength);

        if (nextLeftIndex == -1 || nextRightIndex == -1) {
            QString afterTagString = songOutputFormat.mid(currentCharIndex, songOutputFormat.length() - currentCharIndex);
            newString.append(afterTagString);

            break;
        }

        QString beforeTagString = songOutputFormat.mid(currentCharIndex, nextLeftIndex - currentCharIndex);
        newString.append(beforeTagString);

        QString tagReplacement = tagHandlerFunc(tag);
        if (tagReplacement.isNull()) {
            return std::make_tuple(tag, NamingError::TagInvalid);
        }

        newString.append(tagReplacement);

        currentCharIndex = nextRightIndex + 1;
    }

    return std::make_tuple(newString, NamingError::None);
}

void SpotifyDownloader::OpenURL(QUrl address, QString windowTitle, QString windowMessage) {

    QMessageBox msg = QMessageBox();
    msg.setWindowTitle(windowTitle);
    msg.setText(QString("%1\nThis will open in your browser").arg(windowMessage));
    msg.setIcon(QMessageBox::Question);
    msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    int reply = msg.exec();

    if(reply == QMessageBox::Yes)
        QDesktopServices::openUrl(address);
}

void SpotifyDownloader::closeEvent(QCloseEvent* closeEvent) {
    if (DownloadStarted && !DownloadComplete) {
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
    Paused = false;

    _trayIcon->hide();

    delete _buttonHoverWatcher;

    emit RequestQuit();
    workerThread.wait();
}