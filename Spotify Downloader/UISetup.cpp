#include "SpotifyDownloader.h"

#include <QTemporaryFile>
#include <QClipboard>

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
    _objectHoverWatcher->AddObjectFunctions(_ui.DownloadingScreenButton, [=](QObject* object) {
        _ui.DownloadingScreenButton->setIcon(QIcon(":/SpotifyDownloader/Icons/Download_Icon_W_Filled.png"));
    }, [=](QObject* object) {
        // If not in a downloading screen, reset icon
        int currentScreen = CurrentScreen();
        if (currentScreen != SETUP_SCREEN_INDEX && currentScreen != PROCESSING_SCREEN_INDEX)
            _ui.DownloadingScreenButton->setIcon(QIcon(":/SpotifyDownloader/Icons/Download_Icon_W.png"));
    });

    _objectHoverWatcher->AddObjectFunctions(_ui.SettingsScreenButton, [=](QObject* object) {
        _ui.SettingsScreenButton->setIcon(QIcon(":/SpotifyDownloader/Icons/SettingsCog_W_Filled.png"));
    }, [=](QObject* object) {
        // If not in settings screen, reset icon
        if(CurrentScreen() != SETTINGS_SCREEN_INDEX)
            _ui.SettingsScreenButton->setIcon(QIcon(":/SpotifyDownloader/Icons/SettingsCog_W.png"));
    });

    _objectHoverWatcher->AddObjectFunctions(_ui.ErrorScreenButton, [=](QObject* object) {
        if(_errorUI.count() > 0)
            _ui.ErrorScreenButton->setIcon(QIcon(":/SpotifyDownloader/Icons/Error_Icon_W_Filled.png"));
    }, [=](QObject* object) {
        // If not in error screen, reset icon
        if(_errorUI.count() > 0 && CurrentScreen() != ERROR_SCREEN_INDEX)
            _ui.ErrorScreenButton->setIcon(QIcon(":/SpotifyDownloader/Icons/Error_Icon_W.png"));
    });

    _objectHoverWatcher->AddObjectFunctions(_ui.DonateButton, [=](QObject* object) {
        Animation::AnimateBackgroundColour(_ui.DonateButton, QColor(80, 80, 80), 500);
    }, [=](QObject* object) {
        Animation::AnimateBackgroundColour(_ui.DonateButton, QColor(44, 44, 44), 500);
    });

    _objectHoverWatcher->AddObjectFunctions(_ui.SubmitBugButton, [=](QObject* object) {
        Animation::AnimateBackgroundColour(_ui.SubmitBugButton, QColor(80, 80, 80), 500);
    }, [=](QObject* object) {
        Animation::AnimateBackgroundColour(_ui.SubmitBugButton, QColor(44, 44, 44), 500);
    });

    _objectHoverWatcher->AddObjectFunctions(_ui.HelpButton, [=](QObject* object) {
        Animation::AnimateBackgroundColour(_ui.HelpButton, QColor(80, 80, 80), 500);
    }, [=](QObject* object) {
        Animation::AnimateBackgroundColour(_ui.HelpButton, QColor(44, 44, 44), 500);
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

        // Save Settings
        SaveSettings();

        // Return to downloading screen
        int downloadingScreen = DownloadStarted ? PROCESSING_SCREEN_INDEX : SETUP_SCREEN_INDEX;
        ChangeScreen(downloadingScreen);
    });

    connect(_ui.ErrorScreenButton, &QPushButton::clicked, [=] {
        if (_errorUI.count() <= 0)
            return;

        // Change screen
        ChangeScreen(ERROR_SCREEN_INDEX);
    });

    connect(_ui.SettingsScreenButton, &QPushButton::clicked, [=] {
        // Change screen
        ChangeScreen(SETTINGS_SCREEN_INDEX);
    });

    connect(_ui.DonateButton, &QPushButton::clicked, [=] {
        OpenURL(QUrl("https://ko-fi.com/williamschack"), "Would you like to donate?", "Your donation will support this programs development\nand help it flourish with new features and community support\nfor the foreseeable future!");
    });

    connect(_ui.SubmitBugButton, &QPushButton::clicked, [=] {
        OpenURL(QUrl("https://github.com/WilliamSchack/Spotify-Downloader/issues/new"), "Submit Bug", "Would you like to submit a bug?");
    });

    connect(_ui.HelpButton, &QPushButton::clicked, [=] {
        OpenURL(QUrl("https://github.com/WilliamSchack/Spotify-Downloader?tab=readme-ov-file#usage"), "Help", "Would you like to access the help documentation?");
    });
}

void SpotifyDownloader::SetupSetupScreen() {
    connect(_ui.PasteButton, &QPushButton::clicked, [=] {
        QClipboard* clipboard = qApp->clipboard();
        _ui.PlaylistURLInput->setText(clipboard->text());
    });
    
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
            Paused = false;

            // Setup and Reset GUI
            ChangeScreen(PROCESSING_SCREEN_INDEX);
            _ui.DownloaderThreadsInput->setEnabled(false);

            _ui.PlayButton->hide();
            _ui.PauseButton->show();
            _ui.PauseWarning->hide();

            SetDownloadStatus("");

            qInfo() << "Starting download for playlist" << PlaylistURLText;
            Logger::Flush();

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

    // Set version label
    _ui.VersionLabel->setText(QString("v%1").arg(VERSION));
}

void SpotifyDownloader::SetupSettingsScreen() {
    // Top Buttons
    QList<QPushButton*> topButtons ({
        _ui.OutputSettingsButton,
        _ui.DownloadingSettingsButton,
        _ui.InterfaceSettingsButton
    });

    for (int i = 0; i < topButtons.count(); i++) {
        // i = screenIndex

        QPushButton* button = topButtons[i];
        QPoint buttonPos = button->pos();

        // Hover
        _objectHoverWatcher->AddObjectFunctions(button, [=](QObject* object) {
            if (_ui.SettingsScreens->currentIndex() != i)
                Animation::AnimatePosition(button, QPoint(buttonPos.x(), 12), 200);
        }, [=](QObject* object) {
            if (_ui.SettingsScreens->currentIndex() != i)
                Animation::AnimatePosition(button, QPoint(buttonPos.x(), 14), 200);
        });

        // Click
        connect(button, &QPushButton::clicked, [=] {
            if (_ui.SettingsScreens->currentIndex() != i) {
                // Move down non-clicked buttons
                for (int x = 0; x < topButtons.count(); x++) {
                    if (x == i) continue;

                    QWidget* currentButton = topButtons[x];
                    Animation::AnimatePosition(currentButton, QPoint(currentButton->pos().x(), 14), 200);
                }

                // Move Up Clicked Button
                Animation::AnimatePosition(button, QPoint(buttonPos.x(), 8), 200);

                _ui.SettingsScreens->setCurrentIndex(i);
            }
        });
    }

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

    // Set folder organisation dropdown items font size to 12, cannot do in stylesheet
    QFont folderSortingItemFont = _ui.FolderSortingInput->font();
    folderSortingItemFont.setPointSizeF(12);
    for (int i = 0; i < _ui.FolderSortingInput->count(); i++) {
        _ui.FolderSortingInput->setItemData(i, QVariant(folderSortingItemFont), Qt::FontRole);
    }

    // Set combo box variables on change
    connect(_ui.FolderSortingInput, &QComboBox::currentIndexChanged, [=](int index) { FolderSortingIndex = index; });
    connect(_ui.DownloaderThreadUIInput, &QComboBox::currentIndexChanged, [=](int index) { DownloaderThreadUIIndex = index; });

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
        if (DownloadStarted) {
            QMessageBox messageBox;
            messageBox.setWindowIcon(QIcon(":/SpotifyDownloader/Icon.ico"));
            messageBox.setWindowTitle("Are You Sure?");
            messageBox.setText(QString("Only %1 more to go!").arg(_totalSongs - _songsCompleted));
            messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            int reply = messageBox.exec();

            if (reply == QMessageBox::Yes) {
                emit DisplayFinalMessage();
                emit RequestQuit();

                _ui.PauseWarning->show();

                qInfo() << "Download Cancelled";
            }
        }
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

bool SpotifyDownloader::eventFilter(QObject* watched, QEvent* event) {
    // Handle line indicators
    if (event->type() == QEvent::MouseMove) {        
        if (CurrentScreen() != SETTINGS_SCREEN_INDEX)
            return false;

        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

        // Transform mouse pos to settings screen
        QPoint mousePos = _ui.SettingsScreens->mapFromGlobal(QCursor::pos());
        int mouseY = mousePos.y() - 25; // Offset by indicator height / 2

        // Only continue if mouse within settings screen
        QRect settingsScreensGeometry = _ui.SettingsScreens->geometry();
        if (mousePos.x() < settingsScreensGeometry.x() || mousePos.x() > settingsScreensGeometry.x() + settingsScreensGeometry.width())
            return false;

        // Round y to closest setting
        int newY = ((mouseY + 50 / 2) / 50) * 50; // Round y to closest 50
        newY += 5; // Offset by 5

        // Get current screen line indicator and max height
        QPair<QWidget*, int> lineIndicatorPair = SETTINGS_LINE_INDICATORS()[_ui.SettingsScreens->currentIndex()];
        QWidget* lineIndicator = lineIndicatorPair.first;
        int maxHeight = lineIndicatorPair.second;
        
        // Clamp y to settings in menu
        newY = std::clamp(newY, 5, maxHeight);
        
        // Dont animate if already in position
        QPoint lineIndicatorPos = lineIndicator->pos();
        if (lineIndicatorPos.y() != newY) {
            // Animate to new position
            QPoint newPos(lineIndicatorPos.x(), newY);
            Animation::AnimatePosition(lineIndicator, newPos, 300);
        }
    }

    return false;
}