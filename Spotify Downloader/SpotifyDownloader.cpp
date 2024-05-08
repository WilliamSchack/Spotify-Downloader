#include "SpotifyDownloader.h"

#include "CustomWidgets.h"

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
        _ui.Screens->setCurrentIndex(4);
    });

    connect(_ui.BrowseButton, &QPushButton::clicked, [=] {
        QString directory = QFileDialog::getExistingDirectory(this, tr("Choose Save Location"),"",QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if (directory != "") _ui.SaveLocationInput->setPlainText(directory);
    });

    connect(_ui.ContinueButton, &QPushButton::clicked, [=] {
        PlaylistURLText = _ui.PlaylistURLInput->toPlainText();
        SaveLocationText = _ui.SaveLocationInput->toPlainText();

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

            // Save directory to QSettings
            QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
            settings.beginGroup("Output");
            settings.setValue("saveLocation", SaveLocationText);
            settings.endGroup();

            DownloadStarted = true;

            _ui.Screens->setCurrentIndex(1);
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
    // Set default variables
    ThreadCount = _ui.DownloaderThreadsInput->value();
    NormalizeAudioVolume = _ui.NormalizeVolumeSettingInput->value();

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
    connect(_ui.BackButton, &QPushButton::clicked, [=] {
        SaveSettings();
        
        if(DownloadStarted) _ui.Screens->setCurrentIndex(1); // Change to processing screen
        else _ui.Screens->setCurrentIndex(0); // Change to setup screen
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
        _ui.Screens->setCurrentIndex(4); // Change to settings screen
    });
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

void SpotifyDownloader::SaveSettings() {
    // Get values from UI
    bool overwriteEnabled = _ui.OverwriteSettingButton->isChecked;
    bool normalizeEnabled = _ui.NormalizeVolumeSettingButton->isChecked;
    float normalizeVolume = _ui.NormalizeVolumeSettingInput->value();
    int audioBitrate = _ui.AudioBitrateInput->value();
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
    settings.endGroup();

    settings.beginGroup("Downloading");
    settings.setValue("statusNotificationsEnabled", statusNotificationsEnabled);
    settings.setValue("downloaderThreads", downloaderThreads);
    settings.setValue("downloadSpeedLimit", downloadSpeedLimit);
    settings.endGroup();
}

void SpotifyDownloader::LoadSettings() {
    // Clicking buttons to call their callbacks for settings variables etc.
    // Using default values instead of existence checking

    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);

    settings.beginGroup("Output");

    bool overwriteEnabled = settings.value("overwriteEnabled", false).toBool();
    if(_ui.OverwriteSettingButton->isChecked != overwriteEnabled)
        _ui.OverwriteSettingButton->click();

    bool normalizeEnabled = settings.value("normalizeEnabled", true).toBool();
    if (_ui.NormalizeVolumeSettingButton->isChecked != normalizeEnabled)
        _ui.NormalizeVolumeSettingButton->click();

    float normalizeVolume = settings.value("normalizeVolume", 14.0).toFloat();
    _ui.NormalizeVolumeSettingInput->setValue(normalizeVolume);

    int audioBitrate = settings.value("audioBitrate", 192).toInt();
    _ui.AudioBitrateInput->setValue(audioBitrate);

    QString saveLocation = settings.value("saveLocation").toString();
    _ui.SaveLocationInput->setPlainText(saveLocation);

    settings.endGroup();

    settings.beginGroup("Downloading");

    bool statusNotificationsEnabled = settings.value("statusNotificationsEnabled", true).toBool();
    if (_ui.NotificationSettingButton->isChecked != statusNotificationsEnabled)
        _ui.NotificationSettingButton->click();

    int downloaderThreads = settings.value("downloaderThreads", 3).toInt();
    _ui.DownloaderThreadsInput->setValue(downloaderThreads);

    float downloadSpeedLimit = settings.value("downloadSpeedLimit", 0.0).toFloat();
    _ui.DownloadSpeedSettingInput->setValue(downloadSpeedLimit);

    settings.endGroup();
}

void SpotifyDownloader::SetupDownloaderThread() {
    // Get thread ready to be started
    _playlistDownloader = new PlaylistDownloader();
    _playlistDownloader->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, _playlistDownloader, &QObject::deleteLater);
    connect(this, &SpotifyDownloader::operate, _playlistDownloader, &PlaylistDownloader::DownloadSongs);
    connect(this, &SpotifyDownloader::RequestQuit, _playlistDownloader, &PlaylistDownloader::Quit);

    // Allow thread to access ui elements
    connect(_playlistDownloader, &PlaylistDownloader::SetupUI, this, &SpotifyDownloader::SetupUI);
    connect(_playlistDownloader, &PlaylistDownloader::ChangeScreen, this, &SpotifyDownloader::ChangeScreen);
    connect(_playlistDownloader, &PlaylistDownloader::ShowMessage, this, &SpotifyDownloader::ShowMessage);
    connect(_playlistDownloader, &PlaylistDownloader::SetSongCount, this, &SpotifyDownloader::SetSongCount);
    connect(_playlistDownloader, &PlaylistDownloader::SetErrorItems, this, &SpotifyDownloader::SetErrorItems);
    connect(_playlistDownloader, &PlaylistDownloader::SetThreadFinished, this, &SpotifyDownloader::SetThreadFinished);
}

// Application Exit
SpotifyDownloader::~SpotifyDownloader()
{
    Paused = false;

    _trayIcon->hide();

    emit RequestQuit();
    workerThread.wait();
}