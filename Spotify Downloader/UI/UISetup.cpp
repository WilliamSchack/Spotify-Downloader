#include "SpotifyDownloader.h"

#include <QTemporaryFile>
#include <QClipboard>

#include <QScrollBar.h>

#include "Utilities/Animation.h"

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
        _ui.DownloadingScreenButton->setIcon(Config::DownloadIconFilled());
    }, [=](QObject* object) {
        // If not in a downloading screen, reset icon
        int currentScreen = CurrentScreen();
        if (currentScreen != Config::SETUP_SCREEN_INDEX && currentScreen != Config::PROCESSING_SCREEN_INDEX)
            _ui.DownloadingScreenButton->setIcon(Config::DownloadIcon());
    });

    _objectHoverWatcher->AddObjectFunctions(_ui.SettingsScreenButton, [=](QObject* object) {
        _ui.SettingsScreenButton->setIcon(Config::SettingsIconFilled());
    }, [=](QObject* object) {
        // If not in settings screen, reset icon
        if(CurrentScreen() != Config::SETTINGS_SCREEN_INDEX)
            _ui.SettingsScreenButton->setIcon(Config::SettingsIcon());
    });

    _objectHoverWatcher->AddObjectFunctions(_ui.ErrorScreenButton, [=](QObject* object) {
        if(_errorUI.count() > 0)
            _ui.ErrorScreenButton->setIcon(Config::ErrorIconFilled());
    }, [=](QObject* object) {
        // If not in error screen, reset icon
        if(_errorUI.count() > 0 && CurrentScreen() != Config::ERROR_SCREEN_INDEX)
            _ui.ErrorScreenButton->setIcon(Config::ErrorIcon());
    });

    // Lower buttons
    QList<QPushButton*> lowerSidebarButtons{
        _ui.DonateButton,
        _ui.UpdateButton,
        _ui.SubmitBugButton,
        _ui.HelpButton,
    };

    foreach(QPushButton* button, lowerSidebarButtons) {
        _objectHoverWatcher->AddObjectFunctions(button, [=](QObject* object) {
            Animation::AnimateBackgroundColour(button, QColor(80, 80, 80), 500);
        }, [=](QObject* object) {
            Animation::AnimateBackgroundColour(button, QColor(44, 44, 44), 500);
        });
    }

    // Buttons
    connect(_ui.DownloadingScreenButton, &QPushButton::clicked, [=] {
        if (CurrentScreen() == Config::SETUP_SCREEN_INDEX)
            return;

        // Set output format
        Config::FileNameTag = _ui.FileNameTagInput->text();
        Config::FileName = _ui.FileNameInput->text();

        // Set sub folders
        Config::SubFoldersTag = _ui.SubFoldersTagInput->text();
        Config::SubFolders = _ui.SubFoldersInput->text();

        // Check if audio naming & sub folders are valid
        if (!ValidateSettings())
            return;

        // Save Settings
        Config::SaveSettings();

        // Return to downloading screen
        int downloadingScreen = DownloadStarted ? Config::PROCESSING_SCREEN_INDEX : Config::SETUP_SCREEN_INDEX;
        ChangeScreen(downloadingScreen);
    });

    connect(_ui.ErrorScreenButton, &QPushButton::clicked, [=] {
        if (_errorUI.count() <= 0)
            return;

        // Change screen
        ChangeScreen(Config::ERROR_SCREEN_INDEX);
    });

    connect(_ui.SettingsScreenButton, &QPushButton::clicked, [=] {
        // Change screen
        ChangeScreen(Config::SETTINGS_SCREEN_INDEX);
    });

    connect(_ui.DonateButton, &QPushButton::clicked, [=] {
        OpenURL(QUrl("https://ko-fi.com/williamschack"), "Donate", "Would you like to donate and support development?\nEven the smallest amount goes a long way :)");
    });

    _ui.UpdateImageLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    connect(_ui.UpdateButton, &QPushButton::clicked, [=] {
        CheckForUpdate();

        if (VersionManager::LatestVersion().isEmpty())
            ShowMessageBox(
                "Cannot Check For Updates",
                "Cannot connect to github servers to check for updates",
                QMessageBox::Warning
            );
        else if (VersionManager::UpdateAvailable())
            OpenURL(
                QUrl("https://github.com/WilliamSchack/Spotify-Downloader/releases/latest"),
                "Update Available",
                QString("An Update is available, would you like to update from v%1 to %2?").arg(VersionManager::VERSION).arg(VersionManager::LatestVersion())
            );
        else
            ShowMessageBox(
                "Up To Date",
                "You are on the latest version, no updates required!",
                QMessageBox::Information
            );
    });

    connect(_ui.SubmitBugButton, &QPushButton::clicked, [=] {
        OpenURL(QUrl("https://github.com/WilliamSchack/Spotify-Downloader/issues/new"), "Submit Bug", "Would you like to submit a bug?");
    });

    connect(_ui.HelpButton, &QPushButton::clicked, [=] {
        OpenURL(QUrl("https://github.com/WilliamSchack/Spotify-Downloader?tab=readme-ov-file#usage"), "Help", "Would you like to access the help documentation?");
    });
}

void SpotifyDownloader::SetupSetupScreen() {
    // Disable status and donation prompt
    _ui.DownloadedStatusLabel->setVisible(false);
    _ui.DonationPromptButton->setVisible(false);

    // Set version label
    _ui.VersionLabel->setText(QString("v%1").arg(VersionManager::VERSION));

    connect(_ui.PasteButton, &QPushButton::clicked, [=] {
        QClipboard* clipboard = qApp->clipboard();
        _ui.PlaylistURLInput->setText(clipboard->text());
    });
    
    connect(_ui.BrowseButton, &QPushButton::clicked, [=] {
        QString directory = QFileDialog::getExistingDirectory(this, tr("Choose Save Location"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if (directory != "") _ui.SaveLocationInput->setText(directory);
    });

    connect(_ui.ContinueButton, &QPushButton::clicked, [=] {
        Config::PlaylistURL = _ui.PlaylistURLInput->text();
        Config::SaveLocation = _ui.SaveLocationInput->text();

        if (Config::PlaylistURL != "" && Config::SaveLocation != "") {
            // Validate URL and Directory
            if (!ValidateInputs())
                return;

            // Save directory to QSettings
            QSettings settings(Config::ORGANIZATION_NAME, Config::APPLICATION_NAME);
            settings.beginGroup("Output");
            settings.setValue("saveLocation", Config::SaveLocation);
            settings.endGroup();

            settings.beginGroup("Downloading");

            // Save youtube cookies and po token
            QString youtubeCookies = _ui.YoutubeCookiesInput->text();
            QString poToken = _ui.POTokenInput->text();

            settings.setValue("youtubeCookies", youtubeCookies);
            settings.setValue("poToken", poToken);

            Config::YouTubeCookies = youtubeCookies;

            // Save spotify api keys
            QByteArray clientID = _ui.SpotifyClientIDInput->text().toUtf8();
            QByteArray clientSecret = _ui.SpotifyClientSecretInput->text().toUtf8();

            settings.setValue("clientID", clientID);
            settings.setValue("clientSecret", clientSecret);
            
            settings.endGroup();

            SpotifyAPI::ClientID = clientID;
            SpotifyAPI::ClientSecret = clientSecret;

            // Create cookies file if needed
            if (!youtubeCookies.isEmpty()) {
                // Access/Create file
                QString tempFolder = QString("%1/SpotifyDownloader").arg(QDir::temp().path());
                QString cookiesFilePath = QString("%1/cookies.txt").arg(tempFolder);
                
                QFile cookiesFile(cookiesFilePath);
                cookiesFile.open(QIODevice::WriteOnly);

                // Clear file of any contents
                cookiesFile.resize(0);

                // Write cookies
                QTextStream out(&cookiesFile);
                out << Config::YouTubeCookies;
            }

            // Initialize variables
            DownloadStarted = true;
            Paused = false;

            // Reset HasPremium, will be set after first download
            // Would check before download but that would require a preset video to test so I may aswell test the first download
            Config::HasPremium = true;

            // Setup and Reset GUI
            ChangeScreen(Config::PROCESSING_SCREEN_INDEX);
            _ui.DownloaderThreadsInput->setEnabled(false);
            _ui.YoutubeCookiesInput->setEnabled(false);
            _ui.POTokenInput->setEnabled(false);

            _ui.PlayButton->hide();
            _ui.PauseButton->show();
            _ui.PauseWarning->hide();

            SetDownloadStatus("");

            qInfo() << "Starting download for playlist" << Config::PlaylistURL;

            // Start thread
            workerThread.start();
            emit operate(this);
        }
        else {
            ShowMessageBox(
                "Fields Incorrect",
                "Please Input Both Fields",
                QMessageBox::Warning
            );
        }
    });

    // Donation prompt button
    _objectHoverWatcher->AddObjectFunctions(_ui.DonationPromptButton, [=](QObject* object) {
        Animation::AnimateBackgroundColour(_ui.DonationPromptButton, QColor(80, 80, 80), 500);
    }, [=](QObject* object) {
        Animation::AnimateBackgroundColour(_ui.DonationPromptButton, QColor(65, 65, 65), 500);
    });

    connect(_ui.DonationPromptButton, &QPushButton::clicked, [=] {
        _ui.DonateButton->click();
    });
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
    connect(_ui.NormalizeVolumeSettingInput, &QDoubleSpinBox::textChanged, [=] {
        if (_ui.NormalizeVolumeSettingInput->text() != "") {
            Config::NormalizeAudioVolume = _ui.NormalizeVolumeSettingInput->value();
        }
    });
    connect(_ui.DownloaderThreadsInput, &QSpinBox::textChanged, [=] {
        if (_ui.DownloaderThreadsInput->text() != "") {
            Config::ThreadCount = _ui.DownloaderThreadsInput->value();
        }
    });
    connect(_ui.DownloadSpeedSettingInput, &QDoubleSpinBox::textChanged, [=] {
        if (_ui.DownloadSpeedSettingInput->text() != "") {
            Config::DownloadSpeed = _ui.DownloadSpeedSettingInput->value();
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

            Config::SetBitrate(audioBitrate);

            float estimatedFileSize = Codec::Data[Config::Codec].CalculateFileSize(audioBitrate, 60);
            QString fileSizeText = QString("%1MB/min").arg(QString::number(estimatedFileSize, 'f', 2));
            _ui.AudioBitrateFileSizeLabel_Value->setText(fileSizeText);
        }
    });

    // Set combo box dropdown items font size to 12, cannot do in stylesheet
    QList<QComboBox*> dropdownWidgets {
        _ui.CodecInput,
        _ui.DownloaderThreadUIInput
    };

    QFont dropdownItemFont = dropdownWidgets[0]->font();
    dropdownItemFont.setPointSizeF(12);
    foreach(QComboBox* dropdown, dropdownWidgets) {
        for (int i = 0; i < dropdown->count(); i++) {
            dropdown->setItemData(i, QVariant(dropdownItemFont), Qt::FontRole);
        }
    }

    // Set combo box variables on change
    connect(_ui.CodecInput, &QComboBox::currentIndexChanged, [=](int index) {
        // Set codec
        Config::SetCodecIndex(index);

        // Update bitrate input
        UpdateBitrateInput(Config::Codec);

        // Disable bitrate input if it cannot be changed
        bool bitrateLocked = Codec::Data[Config::Codec].LockedBitrate;
        _ui.AudioBitrateInput->setEnabled(!Codec::Data[Config::Codec].LockedBitrate);

        // Set bitrate input if it can be changed
        if (!bitrateLocked)
            _ui.AudioBitrateInput->setValue(Config::GetBitrate());

        // Update codec details
        QString codecDetails = Codec::Data[Config::Codec].CodecDetails;
        _ui.CodecDetailsLabel->setText(codecDetails);

        // Update visibility and line indicator height if changed
        _ui.CodecDetailsLabel->setVisible(!codecDetails.isEmpty());

        // Would update line indicator height here but it somehow updates itself
        // I got no clue but if it works it works

        // Update file size text
        float estimatedFileSize = Codec::Data[Config::Codec].CalculateFileSize(Config::GetBitrate(), 60);
        QString fileSizeText = QString("%1MB/min").arg(QString::number(estimatedFileSize, 'f', 2));
        _ui.AudioBitrateFileSizeLabel_Value->setText(fileSizeText);
    });

    connect(_ui.TrackNumberInput, &QComboBox::currentIndexChanged, [=](int index) {
        Config::TrackNumberIndex = index;

        // Update warning if set to playlist track number and line indicator height if changed
        bool wasVisible = _ui.CodecDetailsLabel->isVisible();
        bool visible = index == 0;

        _ui.TrackNumberWarningLabel->setVisible(visible);

        // Add height if enabled, remove height if disabled
        if (wasVisible != visible) {
            if (visible)
                _settingsLineIndicatorsCache[0].MaxHeight += 50;
            else
                _settingsLineIndicatorsCache[0].MaxHeight -= 50;
        }
    });

    connect(_ui.DownloaderThreadUIInput, &QComboBox::currentIndexChanged, [=](int index) { Config::DownloaderThreadUIIndex = index; });

    // Check if cookies changed, update bitrate input if empty or not
    connect(_ui.YoutubeCookiesInput, &QLineEdit::textChanged, [=](QString text) {
        UpdateBitrateInput(Config::Codec);
    });

    // Button Clicks (Using isChecked to help with loading settings)
    connect(_ui.OverwriteSettingButton, &CheckBox::clicked, [=] { Config::Overwrite = _ui.OverwriteSettingButton->isChecked; });
    connect(_ui.NotificationSettingButton, &CheckBox::clicked, [=] { Config::Notifications = _ui.NotificationSettingButton->isChecked; });
    connect(_ui.NormalizeVolumeSettingButton, &CheckBox::clicked, [=] {
        Config::NormalizeAudio = _ui.NormalizeVolumeSettingButton->isChecked;
        _ui.NormalizeVolumeSettingInput->setEnabled(Config::NormalizeAudio);
    });
    connect(_ui.SidebarIconsColourButton, &CheckBox::clicked, [=] {
        Config::SidebarIconsColour = _ui.SidebarIconsColourButton->isChecked;

        // Set icons
        _ui.DownloadingScreenButton->setIcon(Config::DownloadIcon());
        if(_errorUI.count() > 0) _ui.ErrorScreenButton->setIcon(Config::ErrorIcon()); // Only set if error screen is active
        _ui.SettingsScreenButton->setIcon(Config::SettingsIconFilled()); // Set to filled, current screen will be settings, user clicked the button here
        _ui.DonateButton->setIcon(Config::DonateIcon());
        _ui.SubmitBugButton->setIcon(Config::BugIcon());
        _ui.HelpButton->setIcon(Config::HelpIcon());

        if (VersionManager::LatestVersion().isEmpty())
            _ui.UpdateImageLabel->setPixmap(Config::UpdateIcon());
        else if (VersionManager::UpdateAvailable())
            _ui.UpdateImageLabel->setPixmap(Config::UpdateAvailableIcon());
        else
            _ui.UpdateImageLabel->setPixmap(Config::UpdateUpToDateIcon());
    });
    connect(_ui.CheckForUpdatesButton, &CheckBox::clicked, [=] { Config::CheckForUpdates = _ui.CheckForUpdatesButton->isChecked; });

    // Paste Buttons
    connect(_ui.YoutubeCookiesPasteButton, &QPushButton::clicked, [=] {
        QClipboard* clipboard = qApp->clipboard();
        _ui.YoutubeCookiesInput->setText(clipboard->text());
    });

    connect(_ui.POTokenPasteButton, &QPushButton::clicked, [=] {
        QClipboard* clipboard = qApp->clipboard();
        _ui.POTokenInput->setText(clipboard->text());
    });

    connect(_ui.SpotifyAPIClientPasteButton, &QPushButton::clicked, [=] {
        QClipboard* clipboard = qApp->clipboard();
        _ui.SpotifyClientIDInput->setText(clipboard->text());
    });

    connect(_ui.SpotifyAPISecretPasteButton, &QPushButton::clicked, [=] {
        QClipboard* clipboard = qApp->clipboard();
        _ui.SpotifyClientSecretInput->setText(clipboard->text());
    });

    // Help Buttons
    connect(_ui.YoutubeCookiesHelpButton, &QPushButton::clicked, [=]{
        OpenURL(QUrl("https://github.com/WilliamSchack/Spotify-Downloader?tab=readme-ov-file#usage"), "Help", "YouTube Cookies are not required. It is used with the PO Token to remove age restrictions and access YouTube Premium quality. Would you like to access the help documentation?");
    });

    connect(_ui.POTokenHelpButton, &QPushButton::clicked, [=] {
        OpenURL(QUrl("https://github.com/WilliamSchack/Spotify-Downloader?tab=readme-ov-file#usage"), "Help", "A PO Token is not required. It is used with the Youtube Cookies to remove age restrictions and access YouTube Premium quality. Would you like to access the help documentation?");
    });

    connect(_ui.SpotifyAPIClientHelpButton, &QPushButton::clicked, [=] {
        OpenURL(QUrl("https://github.com/WilliamSchack/Spotify-Downloader?tab=readme-ov-file#usage"), "Help", "Spotify API Keys are not required. The Client ID is used with the Secret ID to use your own Spotify API Application. Would you like to access the help documentation?");
     });

    connect(_ui.SpotifyAPISecretHelpButton, &QPushButton::clicked, [=] {
        OpenURL(QUrl("https://github.com/WilliamSchack/Spotify-Downloader?tab=readme-ov-file#usage"), "Help", "Spotify API Keys are not required. The Secret ID is used with the Client ID to use your own Spotify API Application. Would you like to access the help documentation?");
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

void SpotifyDownloader::LoadSettingsUI() {
    // Clicking buttons to call their callbacks
    
    // Calculate initial line indicator heights
    SettingsLineIndicators();

    // Overwrite
    if(_ui.OverwriteSettingButton->isChecked != Config::Overwrite)
        _ui.OverwriteSettingButton->click();
    
    // Codec
    _ui.CodecInput->setCurrentIndex(Config::CodecIndex());

    // Codec details
    QString codecDetails = Codec::Data[Config::Codec].CodecDetails;
    _ui.CodecDetailsLabel->setVisible(!codecDetails.isEmpty());
    _ui.CodecDetailsLabel->setText(codecDetails);

    // Remove height for line indicator if disabled
    if(codecDetails.isEmpty())
        _settingsLineIndicatorsCache[0].MaxHeight -= 50;

    // Normalize Volume
    if (_ui.NormalizeVolumeSettingButton->isChecked != Config::NormalizeAudio)
        _ui.NormalizeVolumeSettingButton->click();
    
    _ui.NormalizeVolumeSettingInput->setValue(Config::NormalizeAudioVolume);
    
    // Audio Bitrate
    UpdateBitrateInput(Config::Codec);
    _ui.AudioBitrateInput->setValue(Config::GetBitrate());

    // Estimated file size
    float estimatedFileSize = Codec::Data[Config::Codec].CalculateFileSize(Config::GetBitrate(), 60);
    QString fileSizeText = QString("%1MB/min").arg(QString::number(estimatedFileSize, 'f', 2));
    _ui.AudioBitrateFileSizeLabel_Value->setText(fileSizeText);

    // Track Number
    _ui.TrackNumberInput->setCurrentIndex(Config::TrackNumberIndex);

    // Track Number warning, only visible for playlist setting
    _ui.TrackNumberWarningLabel->setVisible(Config::TrackNumberIndex == 0);
    
    // Remove height for line indicator if disabled
    if(Config::TrackNumberIndex != 0)
        _settingsLineIndicatorsCache[0].MaxHeight -= 50;

    // Save Location
    _ui.SaveLocationInput->setText(Config::SaveLocation);
    
    // Song Output Format
    _ui.FileNameTagInput->setText(Config::FileNameTag);
    _ui.FileNameInput->setText(Config::FileName);
    
    // Folder Sorting
    _ui.SubFoldersTagInput->setText(Config::SubFoldersTag);
    _ui.SubFoldersInput->setText(Config::SubFolders);

    // Status Notifications
    if (_ui.NotificationSettingButton->isChecked != Config::Notifications)
        _ui.NotificationSettingButton->click();
    
    // Downloader Threads
    _ui.DownloaderThreadsInput->setValue(Config::ThreadCount);
    
    // Download Speed Limit
    _ui.DownloadSpeedSettingInput->setValue(Config::DownloadSpeed);
    
    // YouTube Cookies
    _ui.YoutubeCookiesInput->setText(Config::YouTubeCookies);

    // PO Token
    _ui.POTokenInput->setText(Config::POToken);

    // Spotify Client ID
    _ui.SpotifyClientIDInput->setText(SpotifyAPI::ClientID);

    // Spotify Client Secret
    _ui.SpotifyClientSecretInput->setText(SpotifyAPI::ClientSecret);

    // Downloader Thread UI
    _ui.DownloaderThreadUIInput->setCurrentIndex(Config::DownloaderThreadUIIndex);

    // Sidebar Icons Colour
    if (_ui.SidebarIconsColourButton->isChecked != Config::SidebarIconsColour)
        _ui.SidebarIconsColourButton->click();

    // Check for updates
    if (_ui.CheckForUpdatesButton->isChecked != Config::CheckForUpdates)
        _ui.CheckForUpdatesButton->click();

    // Set icons colour
    _ui.DownloadingScreenButton->setIcon(Config::DownloadIconFilled()); // Set to filled, current screen will be setup, LoadSettingsUI only called on startup
    _ui.SettingsScreenButton->setIcon(Config::SettingsIcon());
    _ui.DonateButton->setIcon(Config::DonateIcon());
    _ui.UpdateImageLabel->setPixmap(Config::UpdateIcon());
    _ui.SubmitBugButton->setIcon(Config::BugIcon());
    _ui.HelpButton->setIcon(Config::HelpIcon());
}

bool SpotifyDownloader::ValidateSettings() {
    QStringList namingTags = Config::NAMING_TAGS;

    // Output Format
    std::tuple<QString, Config::NamingError> formattedOutputName = Config::FormatStringWithTags(Config::FileNameTag, Config::FileName, [&namingTags](QString tag) -> std::tuple<QString, bool> {
        if (!namingTags.contains(tag.toLower())) {
            return std::make_tuple("", false);
        }
        else
            return std::make_tuple("", true);
    });

    QString formattedOutputNameString = std::get<0>(formattedOutputName);
    Config::NamingError outputNameNamingError = std::get<1>(formattedOutputName);

    if (outputNameNamingError == Config::NamingError::EnclosingTagsInvalid) {
        ShowMessageBox(
            "Invalid Naming Format Tag",
            QString("Formatting tag must have 2 characters (Opening, Closing)\n%1 is invalid.").arg(formattedOutputNameString),
            QMessageBox::Warning
        );
        return false;
    } else if (outputNameNamingError == Config::NamingError::TagInvalid) {
        ShowMessageBox(
            "Invalid Naming Format",
            QString("Invalid Tag Detected:\n%1").arg(formattedOutputNameString),
            QMessageBox::Warning
        );
        return false;
    }

    // Sub Folders

    // Cannot start with directory
    if (Config::SubFolders.startsWith("/") || Config::SubFolders.startsWith("\\")) {
        ShowMessageBox(
            "Invalid Sub Folders",
            R"(Sub Folders cannot start with "/" or "\")",
            QMessageBox::Warning
        );
        return false;
    }
    
    // Cannot end with directory
    if (Config::SubFolders.endsWith("/") || Config::SubFolders.endsWith("\\")) {
        ShowMessageBox(
            "Invalid Sub Folders",
            R"(Sub Folders cannot end with "/" or "\")",
            QMessageBox::Warning
        );
        return false;
    }

    // Cannot contain double slashes
    if (Config::SubFolders.contains("//") || Config::SubFolders.contains("\\\\")) {
        ShowMessageBox(
            "Invalid Sub Folders",
            R"(Sub Folders cannot contain "//" or "\\")",
            QMessageBox::Warning
        );
        return false;
    }

    // Cannot contain /\ or \/
    if (Config::SubFolders.contains("/\\") || Config::SubFolders.contains("\\/")) {
        ShowMessageBox(
            "Invalid Sub Folders",
            R"(Sub Folders cannot contain "/\" or "\/")",
            QMessageBox::Warning
        );
        return false;
    }

    // Check tags
    std::tuple<QString, Config::NamingError> formattedSubFolders = Config::FormatStringWithTags(Config::SubFoldersTag, Config::SubFolders, [&namingTags](QString tag) -> std::tuple<QString, bool> {
        if (!namingTags.contains(tag.toLower())) {
            return std::make_tuple("", false);
        }
        else
            return std::make_tuple("", true);
    });

    QString formattedSubFoldersString = std::get<0>(formattedSubFolders);
    Config::NamingError subFoldersNamingError = std::get<1>(formattedSubFolders);

    if (subFoldersNamingError == Config::NamingError::EnclosingTagsInvalid) {
        ShowMessageBox(
            "Invalid Sub Folders Tag",
            QString("Formatting tag must have 2 characters (Opening, Closing)\n%1 is invalid.").arg(formattedSubFoldersString),
            QMessageBox::Warning
        );
        return false;
    }
    else if (subFoldersNamingError == Config::NamingError::TagInvalid) {
        ShowMessageBox(
            "Invalid Sub Folders",
            QString("Invalid Tag Detected:\n%1").arg(formattedSubFoldersString),
            QMessageBox::Warning
        );
        return false;
    }

    // Youtube Cookies / PO Token, return false if one is set without the other
    if ((!_ui.YoutubeCookiesInput->text().isEmpty() && _ui.POTokenInput->text().isEmpty()) || (_ui.YoutubeCookiesInput->text().isEmpty() && !_ui.POTokenInput->text().isEmpty())) {
        ShowMessageBox(
            "Invalid Youtube Cookies",
            "Both the Youtube Cookies and PO Token must be set",
            QMessageBox::Warning
        );
        return false;
    }

    // Youtube cookies must be in netscape format
    if (!_ui.YoutubeCookiesInput->text().isEmpty()) {
        QString firstLine = _ui.YoutubeCookiesInput->text().split("\n")[0];
        if (firstLine != "# HTTP Cookie File" && firstLine != "# Netscape HTTP Cookie File") {
            // Prompt user to fix
            ShowMessageBox(
                "Invalid Youtube Cookies",
                "Youtube cookies must be in the netscape format",
                QMessageBox::Warning
            );

            return false;
        }
    }

    // Spotify API Keys, return false if one is set without the other
    if ((!_ui.SpotifyClientIDInput->text().isEmpty() && _ui.SpotifyClientSecretInput->text().isEmpty()) || (_ui.SpotifyClientIDInput->text().isEmpty() && !_ui.SpotifyClientSecretInput->text().isEmpty())) {
        ShowMessageBox(
            "Invalid API Key",
            QString("Both the Spotify API Client ID and Secret must be set"),
            QMessageBox::Warning
        );
        return false;
    }

    return true;
}

bool SpotifyDownloader::ValidateInputs() {
    if (!ValidateURL())
        return false;

    if (!ValidateDirectory())
        return false;

    return true;
}

bool SpotifyDownloader::ValidateURL() {
    QString url = Config::PlaylistURL;

    // Check if url is from spotify and is a playlist, track, album, or episode
    if (url.contains("open.spotify.com") && (url.contains("playlist") || url.contains("track") || url.contains("album") || url.contains("episode")))
        return true;

    // Otherwise let the user know that the url is invalid
    ShowMessageBox(
        "Invalid URL",
        "Please Input A Valid URL",
        QMessageBox::Warning
    );
    return false;
}

bool SpotifyDownloader::ValidateDirectory() {
    // Check if Directory is valid
    if (!std::filesystem::exists(reinterpret_cast<const wchar_t*>(Config::SaveLocation.constData()))) {
        ShowMessageBox(
            "Invalid Directory",
            "Please Input A Valid Directory",
            QMessageBox::Warning
        );
        return false;
    }

    // Check permissions of folder, try to create temp file in location, if error occurs, folder is not writable
    QTemporaryFile tempFile;
    if (tempFile.open()) {
        QString tempFilePath = QString("%1/folderChecker.temp").arg(Config::SaveLocation);

        // If rename failed, and error == "Access is denied.", directory requires admin perms
        bool renameSuccessfull = tempFile.rename(tempFilePath);
        QString errorString = tempFile.errorString();
        tempFile.close();

        if (!renameSuccessfull) {
            bool isElevated = IsElevated();

            if (isElevated) {
                ShowMessageBoxWithButtons(
                    "Directory Error",
                    QString("DIR ERROR: %1\nPlease try another folder.").arg(errorString),
                    QMessageBox::Critical,
                    QMessageBox::Ok
                );

                return false;
            }
            else {
                int adminInput = ShowMessageBoxWithButtons(
                    "Directory Error",
                    QString("DIR ERROR: %1\nWould you like to restart with admin permissions?").arg(errorString),
                    QMessageBox::Critical,
                    QMessageBox::Yes | QMessageBox::No
                );

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
    if (event->type() == QEvent::MouseMove || event->type() == QEvent::Wheel) { // Include mouse movement and scrolling
        if (CurrentScreen() != Config::SETTINGS_SCREEN_INDEX)
            return false;

        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

        // Transform mouse pos to settings screen
        QPoint mousePos = _ui.SettingsScreens->mapFromGlobal(QCursor::pos());
        int mouseY = mousePos.y() - 25; // Offset by indicator height / 2

        // Only continue if mouse within settings screen
        QRect settingsScreensGeometry = _ui.SettingsScreens->geometry();
        if (mousePos.x() < settingsScreensGeometry.x() || mousePos.x() > settingsScreensGeometry.x() + settingsScreensGeometry.width())
            return false;

        // Get current screen line indicator data
        LineIndicator lineIndicatorData = SettingsLineIndicators()[_ui.SettingsScreens->currentIndex()];

        // Offset mouse position to scroll
        int scrollOffset = lineIndicatorData.ScrollArea == nullptr ? 0 : lineIndicatorData.ScrollArea->verticalScrollBar()->value() % 50;
        mouseY += scrollOffset;

        // Round y to closest setting
        int newY = ((mouseY + 50 / 2) / 50) * 50; // Round y to closest 50 + scroll offset
        newY += 5; // Offset by 5, spacing from top
        
        // Clamp y to settings in menu
        newY = std::clamp(newY, 5, lineIndicatorData.MaxHeight);

        // Offset indicator to scroll amount
        if (lineIndicatorData.ScrollArea != nullptr) {
            int offset = lineIndicatorData.ScrollArea->verticalScrollBar()->value() % 50;
            newY -= offset;
        }

        // Dont animate if already in position
        QPoint lineIndicatorPos = lineIndicatorData.Indicator->pos();
        if (lineIndicatorPos.y() != newY) {
            // Animate to new position
            QPoint newPos(lineIndicatorPos.x(), newY);
            Animation::AnimatePosition(lineIndicatorData.Indicator, newPos, event->type() == QEvent::Wheel ? 0 : 300); // Snap to position when animating
        }
    }

    return false;
}