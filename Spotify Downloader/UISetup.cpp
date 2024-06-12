#include "SpotifyDownloader.h"

#include <QTemporaryFile>

#include "Animation.h"

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

void SpotifyDownloader::SetupSideBar() {
    // Hover
    _buttonHoverWatcher->AddButtonFunctions(_ui.DownloadingScreenButton, [](QPushButton* button) {
        button->setIcon(QIcon(":/SpotifyDownloader/Icons/Download_Icon_W_Filled.png"));
    }, [=](QPushButton* button) {
        // If not in a downloading screen, reset icon
        if(CurrentScreen() == SETTINGS_SCREEN_INDEX)
            button->setIcon(QIcon(":/SpotifyDownloader/Icons/Download_Icon_W.png"));
    });

    _buttonHoverWatcher->AddButtonFunctions(_ui.SettingsScreenButton, [](QPushButton* button) {
        button->setIcon(QIcon(":/SpotifyDownloader/Icons/SettingsCog_W_Filled.png"));
    }, [=](QPushButton* button) {
        // If not in settings screen, reset icon
        if(CurrentScreen() != SETTINGS_SCREEN_INDEX)
            button->setIcon(QIcon(":/SpotifyDownloader/Icons/SettingsCog_W.png"));
    });

    _buttonHoverWatcher->AddButtonFunctions(_ui.SubmitBugButton, [](QPushButton* button) {
        Animation::AnimateBackgroundColour(button, QColor(80, 80, 80), 500);
    }, [=](QPushButton* button) {
        Animation::AnimateBackgroundColour(button, QColor(44, 44, 44), 500);
    });

    _buttonHoverWatcher->AddButtonFunctions(_ui.HelpButton, [](QPushButton* button) {
        Animation::AnimateBackgroundColour(button, QColor(80, 80, 80), 500);
    }, [=](QPushButton* button) {
        Animation::AnimateBackgroundColour(button, QColor(44, 44, 44), 500);
    });

    // Buttons
    connect(_ui.DownloadingScreenButton, &QPushButton::clicked, [=] {
        if (CurrentScreen() == SETUP_SCREEN_INDEX)
            return;

        // Check if audio naming is valid
        SongOutputFormatTag = _ui.SongOutputFormatTagInput->text();
        SongOutputFormat = _ui.SongOutputFormatInput->text();

        if (!ValidateSettings())
            return;

        // Setup Icons
        _ui.DownloadingScreenButton->setIcon(QIcon(":/SpotifyDownloader/Icons/Download_Icon_W_Filled.png"));
        _ui.SettingsScreenButton->setIcon(QIcon(":/SpotifyDownloader/Icons/SettingsCog_W.png"));
        Animation::AnimatePosition(_ui.SideBar_LineIndicator, QPoint(0, 6), 500);

        // Save Settings
        SaveSettings();

        // Return to previous screen
        ChangeScreen(_previousScreenIndex);
    });

    connect(_ui.SettingsScreenButton, &QPushButton::clicked, [=] {
        // Setup Icons
        _ui.SettingsScreenButton->setIcon(QIcon(":/SpotifyDownloader/Icons/SettingsCog_W_Filled.png"));
        _ui.DownloadingScreenButton->setIcon(QIcon(":/SpotifyDownloader/Icons/Download_Icon_W.png"));
        Animation::AnimatePosition(_ui.SideBar_LineIndicator, QPoint(0, 53), 500);

        // Change screen
        ChangeScreen(SETTINGS_SCREEN_INDEX);
    });

    connect(_ui.SubmitBugButton, &QPushButton::clicked, [=] {
        OpenURL(QUrl("https://github.com/WilliamSchack/Spotify-Downloader/issues/new"), "Submit Bug", "Would you like to submit a bug?");
    });

    connect(_ui.HelpButton, &QPushButton::clicked, [=] {
        OpenURL(QUrl("https://github.com/WilliamSchack/Spotify-Downloader?tab=readme-ov-file#usage"), "Access Help", "Would you like to access the help documentation?");
    });
}

void SpotifyDownloader::SetupSetupScreen() {
    connect(_ui.BrowseButton, &QPushButton::clicked, [=] {
        QString directory = QFileDialog::getExistingDirectory(this, tr("Choose Save Location"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if (directory != "") _ui.SaveLocationInput->setText(directory);
    });

    connect(_ui.ContinueButton, &QPushButton::clicked, [=] {
        PlaylistURLText = _ui.PlaylistURLInput->text();
        SaveLocationText = _ui.SaveLocationInput->text();

        if (PlaylistURLText != "" && SaveLocationText != "") {
            // Validate URL and Directory
            if (!ValidateInputs())
                return;

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
        }
        else {
            QMessageBox msg = QMessageBox();
            msg.setWindowTitle("Fields Incorrect");
            msg.setText("Please Input Both Fields");
            msg.setIcon(QMessageBox::Warning);
            msg.exec();
        }
    });
}

void SpotifyDownloader::SetupSettingsScreen() {
    // Top Buttons
    _buttonHoverWatcher->AddButtonFunctions(_ui.OutputSettingsButton, [=] (QPushButton* button) {
        if (_ui.SettingsScreens->currentIndex() != 0)
            Animation::AnimatePosition(button, QPoint(8, 12), 200);
    }, [=](QPushButton* button) {
        if (_ui.SettingsScreens->currentIndex() != 0)
            Animation::AnimatePosition(button, QPoint(8, 14), 200);
    });

    _buttonHoverWatcher->AddButtonFunctions(_ui.DownloadingSettingsButton, [=](QPushButton* button) {
        if (_ui.SettingsScreens->currentIndex() != 1)
            Animation::AnimatePosition(button, QPoint(371, 12), 200);
    }, [=](QPushButton* button) {
        if(_ui.SettingsScreens->currentIndex() != 1)
            Animation::AnimatePosition(button, QPoint(371, 14), 200);
    });

    connect(_ui.OutputSettingsButton, &QPushButton::clicked, [=] {
        if (_ui.SettingsScreens->currentIndex() != 0) {
            Animation::AnimatePosition(_ui.OutputSettingsButton, QPoint(8, 8), 300);
            Animation::AnimatePosition(_ui.DownloadingSettingsButton, QPoint(371, 14), 200);

            _ui.SettingsScreens->setCurrentIndex(0);
        }
    });

    connect(_ui.DownloadingSettingsButton, &QPushButton::clicked, [=] {
        if (_ui.SettingsScreens->currentIndex() != 1) {
            Animation::AnimatePosition(_ui.DownloadingSettingsButton, QPoint(371, 8), 300);
            Animation::AnimatePosition(_ui.OutputSettingsButton, QPoint(8, 14), 200);

            _ui.SettingsScreens->setCurrentIndex(1);
        }
    });

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
    connect(_ui.CancelButton, &QPushButton::clicked, [=] {
        if (DownloadStarted && !DownloadComplete) {
            QMessageBox messageBox;
            messageBox.setWindowIcon(QIcon(":/SpotifyDownloader/Icon.ico"));
            messageBox.setWindowTitle("Are You Sure?");
            messageBox.setText(QString("Only %1 more to go!").arg(_totalSongs - _songsCompleted));
            messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            int reply = messageBox.exec();

            if (reply == QMessageBox::Yes) {
                emit ChangeScreen(SETUP_SCREEN_INDEX);
                emit DisplayFinalMessage();

                emit RequestQuit();
            }
        }
    });
}

void SpotifyDownloader::SetupErrorScreen() {
    connect(_ui.ErrorBackButton, &QPushButton::clicked, [=] {
        ChangeScreen(SETUP_SCREEN_INDEX);
    });
}

bool SpotifyDownloader::ValidateSettings() {
    QStringList namingTags = Q_NAMING_TAGS();
    std::tuple<QString, NamingError> formattedOutputName = FormatOutputNameWithTags([&namingTags](QString tag) -> QString {
        if (!namingTags.contains(tag.toLower())) {
            return nullptr;
        }
        else
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
        return false;
    } else if (namingError == NamingError::TagInvalid) {
        QMessageBox msg = QMessageBox();
        msg.setWindowTitle("Invalid Naming Format");
        msg.setText(QString("Invalid Tag Detected:\n%1").arg(formattedOutputNameString));
        msg.setIcon(QMessageBox::Warning);
        msg.exec();
        return false;
    }

    return true;
}

bool SpotifyDownloader::ValidateInputs() {
    // Check if both URL and Directory are valid
    if ((!PlaylistURLText.contains("open.spotify.com/playlist/") && !PlaylistURLText.contains("open.spotify.com/track/") && !PlaylistURLText.contains("open.spotify.com/album/")) && !std::filesystem::exists(SaveLocationText.toStdString())) {
        QMessageBox msg = QMessageBox();
        msg.setWindowTitle("Invalid Fields");
        msg.setText("Please Enter Valid Inputs Into Both Fields");
        msg.setIcon(QMessageBox::Warning);
        msg.exec();
        return false;
    }

    if (!ValidateURL())
        return false;

    if (!ValidateDirectory())
        return false;

    return true;
}

bool SpotifyDownloader::ValidateURL() {
    if (!PlaylistURLText.contains("open.spotify.com/playlist/") && !PlaylistURLText.contains("open.spotify.com/track/") && !PlaylistURLText.contains("open.spotify.com/album/")) {
        QMessageBox msg = QMessageBox();
        msg.setWindowTitle("Invalid URL");
        msg.setText("Please Input A Valid URL");
        msg.setIcon(QMessageBox::Warning);
        msg.exec();
        return false;
    }

    return true;
}

bool SpotifyDownloader::ValidateDirectory() {
    // Check if Directory is valid
    if (!std::filesystem::exists(SaveLocationText.toStdString())) {
        QMessageBox msg = QMessageBox();
        msg.setWindowTitle("Invalid Directory");
        msg.setText("Please Input A Valid Directory");
        msg.setIcon(QMessageBox::Warning);
        msg.exec();
        return false;
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

                return false;
            }
            else {
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
                } else if (adminInput == QMessageBox::No) {
                    return false;
                }
            }
        }
    }

    return true;
}