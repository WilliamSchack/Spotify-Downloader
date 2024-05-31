#include "SpotifyDownloader.h"

#include "CustomWidgets.h"

#include <QTemporaryFile>
#include <QDesktopServices>

#include <qt_windows.h>

// Ui Setup
SpotifyDownloader::SpotifyDownloader(QWidget* parent) : QDialog(parent)
{
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);

    QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
    QCoreApplication::setApplicationName(APPLICATION_NAME);

    _ui.setupUi(this);

    SetupTrayIcon();
    SetupSetupScreen();
    SetupSettingsScreen();
    SetupProcessingScreen();
    SetupErrorScreen();

    LoadSettings();

    SetupDownloaderThread();
}

void SpotifyDownloader::SetupTrayIcon() {
    QAction* progressAction = new QAction(tr("Current Progress"));
    connect(progressAction, &QAction::triggered, this, [&] {
        if (DownloadStarted) {
            int completed = _songsCompleted;
            int total = _totalSongs;

            QString message;
            if (completed == 0 || total == 0)
                message = "0.00% Complete";
            else {
                // Percent complete rounded to two decimals
                float percentComplete = std::round(((completed * 1.0) / (total * 1.0) * 100.0) / 0.01) * 0.01;
                QString percentString = QString::number(percentComplete);
                
                // Add .0 at the end if it does not contain to make it consistent
                if (!percentString.contains("."))
                    percentString.append(".00");

                message = QString("%1% Complete").arg(percentString);
            }

            ShowMessage(message, QString("Completed: %1/%2").arg(completed).arg(total));
            return;
        }

        ShowMessage("Not Started Yet", "No progress to show...");
    });

    QAction* quitAction = new QAction(tr("Exit"));
    connect(quitAction, &QAction::triggered, this, &QWidget::close);

    QMenu* contextMenu = new QMenu();
    contextMenu->addAction(progressAction);
    contextMenu->addSeparator();
    contextMenu->addAction(quitAction);

    _trayIcon = new QSystemTrayIcon();
    _trayIcon->setIcon(QIcon(":/SpotifyDownloader/Icon.ico"));
    _trayIcon->setContextMenu(contextMenu);
    connect(_trayIcon, &QSystemTrayIcon::activated, this, [&] {
        show();
    });
    _trayIcon->show();
}

void SpotifyDownloader::SetupSetupScreen() {
    connect(_ui.SettingsButton, &QPushButton::clicked, [=] {
        ChangeScreen(SETTINGS_SCREEN_INDEX);
    });

    connect(_ui.BrowseButton, &QPushButton::clicked, [=] {
        QString directory = QFileDialog::getExistingDirectory(this, tr("Choose Save Location"),"",QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if (directory != "") _ui.SaveLocationInput->setText(directory);
    });

    connect(_ui.SubmitBugButton, &QPushButton::clicked, [=] {
        OpenURL(QUrl("https://github.com/WilliamSchack/Spotify-Downloader/issues/new"), "Submit Bug", "Would you like to submit a bug?");
    });

    connect(_ui.HelpButton, &QPushButton::clicked, [=] {
        OpenURL(QUrl("https://github.com/WilliamSchack/Spotify-Downloader?tab=readme-ov-file#usage"), "Access Help", "Would you like to access the help documentation?");
    });

    connect(_ui.ContinueButton, &QPushButton::clicked, [=] {
        PlaylistURLText = _ui.PlaylistURLInput->text();
        SaveLocationText = _ui.SaveLocationInput->text();

        if (PlaylistURLText != "" && SaveLocationText != "") {
            // Check if both URL and Directory are valid
            if ((!PlaylistURLText.contains("open.spotify.com/playlist/") && !PlaylistURLText.contains("open.spotify.com/track/") && !PlaylistURLText.contains("open.spotify.com/album/")) && !std::filesystem::exists(SaveLocationText.toStdString())) {
                QMessageBox msg = QMessageBox();
                msg.setWindowTitle("Invalid Fields");
                msg.setText("Please Enter Valid Inputs Into Both Fields");
                msg.setIcon(QMessageBox::Warning);
                msg.exec();
                return;
            }

            // Check if URL is valid
            if (!PlaylistURLText.contains("open.spotify.com/playlist/") && !PlaylistURLText.contains("open.spotify.com/track/") && !PlaylistURLText.contains("open.spotify.com/album/")) {
                QMessageBox msg = QMessageBox();
                msg.setWindowTitle("Invalid URL");
                msg.setText("Please Input A Valid URL");
                msg.setIcon(QMessageBox::Warning);
                msg.exec();
                return;
            }

            // Check if Directory is valid
            if (!std::filesystem::exists(SaveLocationText.toStdString())) {
                QMessageBox msg = QMessageBox();
                msg.setWindowTitle("Invalid Directory");
                msg.setText("Please Input A Valid Directory");
                msg.setIcon(QMessageBox::Warning);
                msg.exec();
                return;
            }

            // Check permissions of folder, try to create temp file in location, if error occurs, folder is not writable
            QTemporaryFile tempFile;
            if (tempFile.open()) {
                QString tempFilePath = QString("%1/folderChecker.temp").arg(SaveLocationText);

                // If rename failed, and error == "Access is denied.", directory requires admin perms
                bool renameSuccessfull = tempFile.rename(tempFilePath);
                QString errorString = tempFile.errorString();
                tempFile.close();

                if (!renameSuccessfull) {
                    bool isElevated = IsElevated();

                    if (isElevated) {
                        QMessageBox msg = QMessageBox();
                        msg.setWindowTitle("Directory Error");
                        msg.setText(QString("DIR ERROR: %1\nPlease try another folder.").arg(errorString));
                        msg.setIcon(QMessageBox::Critical);
                        msg.setStandardButtons(QMessageBox::Ok);
                        msg.exec();

                        return;
                    } else {
                        QMessageBox msg = QMessageBox();
                        msg.setWindowTitle("Directory Error");
                        msg.setText(QString("DIR ERROR: %1\nWould you like to restart with admin permissions?").arg(errorString));
                        msg.setIcon(QMessageBox::Critical);
                        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                        int adminInput = msg.exec();

                        if (adminInput == QMessageBox::Yes) {
                            // Restart with admin perms
                            QString exePath = QCoreApplication::applicationFilePath();
                            QStringList args = QStringList({ "-Command", QString("Start-Process '%1' -Verb runAs").arg(exePath) });
                            QProcess* elevatedApplication = new QProcess(this);
                            elevatedApplication->start("powershell", args);

                            // Wait for elevated process to open, then close this one
                            elevatedApplication->waitForFinished();
                            QCoreApplication::quit();
                        }
                        else if (adminInput == QMessageBox::No) {
                            return;
                        }
                    }
                }
            }

            // Save directory to QSettings
            QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
            settings.beginGroup("Output");
            settings.setValue("saveLocation", SaveLocationText);
            settings.endGroup();

            DownloadStarted = true;

            ChangeScreen(PROCESSING_SCREEN_INDEX);
            _ui.DownloaderThreadsInput->setEnabled(false);

            // Start thread
            workerThread.start();
            emit operate(this);
        } else {
            QMessageBox msg = QMessageBox();
            msg.setWindowTitle("Fields Incorrect");
            msg.setText("Please Input Both Fields");
            msg.setIcon(QMessageBox::Warning);
            msg.exec();
        }
    });
}

void SpotifyDownloader::SetupSettingsScreen() {
    // Number Inputs
    connect(_ui.DownloaderThreadsInput, &QSpinBox::textChanged, [=] {
        if (_ui.DownloaderThreadsInput->text() != "") {
            ThreadCount = _ui.DownloaderThreadsInput->value();
        }
    });
    connect(_ui.DownloadSpeedSettingInput, &QDoubleSpinBox::textChanged, [=] {
        if (_ui.DownloadSpeedSettingInput->text() != "") {
            DownloadSpeed = _ui.DownloadSpeedSettingInput->value();
        }
    });
    connect(_ui.NormalizeVolumeSettingInput, &QDoubleSpinBox::textChanged, [=] {
        if (_ui.NormalizeVolumeSettingInput->text() != "") {
            NormalizeAudioVolume = _ui.NormalizeVolumeSettingInput->value();
        }
    });
    connect(_ui.AudioBitrateInput, &QSpinBox::textChanged, [=] {
        if (_ui.AudioBitrateInput->text() != "") {
            int audioBitrate = _ui.AudioBitrateInput->value();

            // Snap value to closest multiple of 32
            if (audioBitrate % 32 != 0) {
                int remainder = audioBitrate % 32;
                if (remainder < 16)
                    audioBitrate -= remainder;
                else
                    audioBitrate += 32 - remainder;

                _ui.AudioBitrateInput->setValue(audioBitrate);
            }

            AudioBitrate = audioBitrate;

            float estimatedFileSize = (((float)AudioBitrate * 60) / 8) / 1024;
            QString fileSizeText = QString("%1MB/min").arg(QString::number(estimatedFileSize, 'f', 2));

            _ui.AudioBitrateFileSizeLabel_Value->setText(fileSizeText);
        }
    });

    // Button Clicks (Using isChecked to help with loading settings)
    connect(_ui.OverwriteSettingButton, &CheckBox::clicked, [=] { Overwrite = _ui.OverwriteSettingButton->isChecked; });
    connect(_ui.NotificationSettingButton, &CheckBox::clicked, [=] { Notifications = _ui.NotificationSettingButton->isChecked; });
    connect(_ui.NormalizeVolumeSettingButton, &CheckBox::clicked, [=] {
        NormalizeAudio = _ui.NormalizeVolumeSettingButton->isChecked;
        _ui.NormalizeVolumeSettingInput->setEnabled(NormalizeAudio);
    });
    connect(_ui.SettingsBackButton, &QPushButton::clicked, [=] {
        // Check if audio naming is valid
        SongOutputFormatTag = _ui.SongOutputFormatTagInput->text();
        SongOutputFormat = _ui.SongOutputFormatInput->text();

        QStringList namingTags = Q_NAMING_TAGS();
        std::tuple<QString, NamingError> formattedOutputName = FormatOutputNameWithTags([&namingTags](QString tag) -> QString {
            if (!namingTags.contains(tag.toLower())) {
                return nullptr;
            } else
                return QString("");
        });

        QString formattedOutputNameString = std::get<0>(formattedOutputName);
        NamingError namingError = std::get<1>(formattedOutputName);

        if (namingError == NamingError::EnclosingTagsInvalid) {
            QMessageBox msg = QMessageBox();
            msg.setWindowTitle("Invalid Naming Format Tag");
            msg.setText(QString("Formatting tag must have 2 characters (Opening, Closing)\n%1 is invalid.").arg(formattedOutputNameString));
            msg.setIcon(QMessageBox::Warning);
            msg.exec();
            return;
        } else if (namingError == NamingError::TagInvalid) {
            QMessageBox msg = QMessageBox();
            msg.setWindowTitle("Invalid Naming Format");
            msg.setText(QString("Invalid Tag Detected:\n%1").arg(formattedOutputNameString));
            msg.setIcon(QMessageBox::Warning);
            msg.exec();
            return;
        }

        // Save Settings
        SaveSettings();
        
        // Return to previous screen
        if(DownloadStarted) ChangeScreen(PROCESSING_SCREEN_INDEX);
        else ChangeScreen(SETUP_SCREEN_INDEX);
    });
}

void SpotifyDownloader::SetupProcessingScreen() {
    // Hide currently uneeded elements
    _ui.PlayButton->hide();
    _ui.PauseWarning->hide();

    // Buttons
    connect(_ui.PauseButton, &QPushButton::clicked, [=] {
        Paused = true;
        _threadsPaused = 0;

        _ui.PauseButton->hide();
        _ui.PlayButton->show();
        _ui.PauseWarning->show();
    });
    connect(_ui.PlayButton, &QPushButton::clicked, [=] {
        Paused = false;

        _ui.PlayButton->hide();
        _ui.PauseButton->show();
        _ui.PauseWarning->hide();
    });
    connect(_ui.SettingsButton_2, &QPushButton::clicked, [=] {
        ChangeScreen(SETTINGS_SCREEN_INDEX);
    });
}

void SpotifyDownloader::SetupErrorScreen() {
    connect(_ui.ErrorBackButton, &QPushButton::clicked, [=] {
        ChangeScreen(SETUP_SCREEN_INDEX);
    });
}

void SpotifyDownloader::SetupDownloaderThread() {
    // Get thread ready to be started
    _playlistDownloader = new PlaylistDownloader();
    _playlistDownloader->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, &workerThread, &QThread::quit);
    connect(&workerThread, &QThread::finished, _playlistDownloader, &PlaylistDownloader::deleteLater);

    connect(this, &SpotifyDownloader::operate, _playlistDownloader, &PlaylistDownloader::DownloadSongs);
    connect(this, &SpotifyDownloader::RequestQuit, _playlistDownloader, &PlaylistDownloader::Quit);

    // Allow thread to access ui elements
    connect(_playlistDownloader, &PlaylistDownloader::SetupUI, this, &SpotifyDownloader::SetupUI);
    connect(_playlistDownloader, &PlaylistDownloader::ChangeScreen, this, &SpotifyDownloader::ChangeScreen);
    connect(_playlistDownloader, &PlaylistDownloader::ShowMessage, this, &SpotifyDownloader::ShowMessage);
    connect(_playlistDownloader, &PlaylistDownloader::SetDownloadStatus, this, &SpotifyDownloader::SetDownloadStatus);
    connect(_playlistDownloader, &PlaylistDownloader::SetSongCount, this, &SpotifyDownloader::SetSongCount);
    connect(_playlistDownloader, &PlaylistDownloader::SetErrorItems, this, &SpotifyDownloader::SetErrorItems);
    connect(_playlistDownloader, &PlaylistDownloader::SetThreadFinished, this, &SpotifyDownloader::SetThreadFinished);
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

    QString songOutputFormat = settings.value("songOutputFormat", "<Song Name> - <Song Artist>").toString();
    _ui.SongOutputFormatInput->setText(songOutputFormat);

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
    // Set Downloading Status
    if (_songsCompleted > 0) {
        int tracksNotFound = _playlistDownloader->TracksNotFound();
        int tracksDownloaded = _songsCompleted - tracksNotFound;
        SetDownloadStatus(QString("Successfully Downloaded %1 Song%2 With %3 Error%4")
            .arg(_songsCompleted - tracksNotFound).arg(tracksDownloaded != 1 ? "s" : "")
            .arg(tracksNotFound).arg(tracksNotFound != 1 ? "s" : ""));
    }
    else {
        SetDownloadStatus("");
    }

    // Reset Variables
    DownloadStarted = false;
    DownloadComplete = false;

    _totalSongs = 0;
    _songsCompleted = 0;

    _playlistDownloader->Quit();

    // Reset UI
    _ui.DownloaderThreadsInput->setEnabled(true);
    _ui.SongCount->setText("0/0");
    _ui.SongCount->adjustSize();

    _downloaderUI.clear(); // Downloader UI is already deleted, just clear list

    if (_errorUI.count() > 0) {
        foreach(SongErrorItem * errorItemUI, _errorUI) {
            if(errorItemUI != nullptr) delete errorItemUI;
        }
        _errorUI.clear();
    }

    // Setup Threads
    SetupDownloaderThread();
}

// Convert naming tags to QStringList, cannot have 
// const QStringList
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

    emit RequestQuit();
    workerThread.wait();
}