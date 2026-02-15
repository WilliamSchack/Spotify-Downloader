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

    _objectHoverWatcher->AddObjectFunctions(_ui.ErrorScreenButton, [=](QObject* object) {
        if(_errorUI.count() > 0)
            _ui.ErrorScreenButton->setIcon(Config::ErrorIconFilled());
    }, [=](QObject* object) {
        // If not in error screen, reset icon
        if(_errorUI.count() > 0 && CurrentScreen() != Config::ERROR_SCREEN_INDEX)
            _ui.ErrorScreenButton->setIcon(Config::ErrorIcon());
    });

    _objectHoverWatcher->AddObjectFunctions(_ui.NoticesScreenButton, [=](QObject* object) {
        _ui.NoticesScreenButton->setIcon(_unreadNotices > 0 ? Config::NoticesAvailableIconFilled() : Config::NoticesIconFilled());
    }, [=](QObject* object) {
        // If not in notices screen, reset icon
        if (CurrentScreen() != Config::NOTICES_SCREEN_INDEX)
            _ui.NoticesScreenButton->setIcon(_unreadNotices > 0 ? Config::NoticesAvailableIcon() : Config::NoticesIcon());
    });

    _objectHoverWatcher->AddObjectFunctions(_ui.SettingsScreenButton, [=](QObject* object) {
        _ui.SettingsScreenButton->setIcon(Config::SettingsIconFilled());
    }, [=](QObject* object) {
        // If not in settings screen, reset icon
        if(CurrentScreen() != Config::SETTINGS_SCREEN_INDEX)
            _ui.SettingsScreenButton->setIcon(Config::SettingsIcon());
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
            Animation::AnimateStylesheetColour(button, "background-color", QColor(80, 80, 80), 500);
        }, [=](QObject* object) {
            Animation::AnimateStylesheetColour(button, "background-color", QColor(44, 44, 44), 500);
        });
    }

    // Buttons
    connect(_ui.DownloadingScreenButton, &QPushButton::clicked, [=] {
        if (CurrentScreen() == Config::SETUP_SCREEN_INDEX)
            return;

        // Set Artist Seperator
        Config::ArtistSeparator = _ui.ArtistSeparatorInput->text();

        // Set output format
        Config::FileNameTag = _ui.FileNameTagInput->text();
        Config::FileName = _ui.FileNameInput->text();

        // Set sub folders
        Config::SubFoldersTag = _ui.SubFoldersTagInput->text();
        Config::SubFolders = _ui.SubFoldersInput->text();

        // Set lrc file
        Config::LRCFileNameTag = _ui.LRCFileNameTagInput->text();
        Config::LRCFileName = _ui.LRCFileNameInput->text();

        // Set playlist file
        Config::PlaylistFileNameTag = _ui.PlaylistFileNameTagInput->text();
        Config::PlaylistFileName = _ui.PlaylistFileNameInput->text();

        // Check if audio naming & sub folders are valid
        if (!ValidateSettings())
            return;

        // Save Settings
        if(CurrentScreen() == Config::SETTINGS_SCREEN_INDEX) Config::SaveSettings();

        // Return to downloading screen
        int downloadingScreen = DownloadStarted ? Config::PROCESSING_SCREEN_INDEX : Config::SETUP_SCREEN_INDEX;
        ChangeScreen(downloadingScreen);
    });

    connect(_ui.ErrorScreenButton, &QPushButton::clicked, [=] {
        if (_errorUI.count() <= 0)
            return;

        // Save Settings
        if (CurrentScreen() == Config::SETTINGS_SCREEN_INDEX) Config::SaveSettings();

        // Change screen
        ChangeScreen(Config::ERROR_SCREEN_INDEX);
    });

    connect(_ui.NoticesScreenButton, &QPushButton::clicked, [=] {
        // Save Settings
        if (CurrentScreen() == Config::SETTINGS_SCREEN_INDEX) Config::SaveSettings();

        // Change screen
        ChangeScreen(Config::NOTICES_SCREEN_INDEX);
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
        OpenURL(QUrl("https://github.com/WilliamSchack/Spotify-Downloader/issues/new/choose"), "Submit Bug", "Would you like to submit a bug?");
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

            // Save spotify api keys
            QByteArray clientID = _ui.SpotifyClientIDInput->text().toUtf8();
            QByteArray clientSecret = _ui.SpotifyClientSecretInput->text().toUtf8();

            settings.beginGroup("Downloading");
            settings.setValue("clientID", clientID);
            settings.setValue("clientSecret", clientSecret);
            settings.endGroup();

            SpotifyAPI::ClientID = clientID;
            SpotifyAPI::ClientSecret = clientSecret;

            // Create cookies file if needed
            if (!Config::YouTubeCookies.isEmpty()) {
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

            // Setup and Reset GUI
            ChangeScreen(Config::PROCESSING_SCREEN_INDEX);
            _ui.DownloaderThreadsInput->setEnabled(false);

            _ui.YoutubeCookiesClearButton->setEnabled(false);
            _ui.YoutubeCookiesPasteButton->setEnabled(false);

            _ui.POTokenInput->setEnabled(false);
            _ui.POTokenClearButton->setEnabled(false);
            _ui.POTokenPasteButton->setEnabled(false);

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
        Animation::AnimateStylesheetColour(_ui.DonationPromptButton, "background-color", QColor(80, 80, 80), 500);
    }, [=](QObject* object) {
        Animation::AnimateStylesheetColour(_ui.DonationPromptButton, "background-color", QColor(65, 65, 65), 500);
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
    connect(_ui.DownloadTimeoutSettingInput, &QSpinBox::textChanged, [=] {
        if (_ui.DownloadTimeoutSettingInput->text() != "") {
            Config::DownloadTimeout = _ui.DownloadTimeoutSettingInput->value();
        }
    });

    connect(_ui.AudioBitrateInput, &QSpinBox::textChanged, [=] {
        if (_ui.AudioBitrateInput->text() != "") {
            int audioBitrate = _ui.AudioBitrateInput->value();

            // Dont update the saved setting if using highest quality bitrate
            if (!Config::AutomaticBestQuality) {
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
            }

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
        _ui.AudioBitrateInput->setEnabled(!Codec::Data[Config::Codec].LockedBitrate && !Config::AutomaticBestQuality);

        // Set bitrate input if it can be changed
        if (!bitrateLocked) {
            if (Config::AutomaticBestQuality) {
                Codec::ExtensionData currentCodecData = Codec::Data[Config::Codec];
                _ui.AudioBitrateInput->setValue(Config::YouTubeCookies.isEmpty() ? currentCodecData.BitrateHighQuality : currentCodecData.BitrateHighQualityPremium);
            } else
                _ui.AudioBitrateInput->setValue(Config::GetBitrate());
        }

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

    connect(_ui.PlaylistFileTypeInput, &QComboBox::currentIndexChanged, [=](int index) { Config::PlaylistFileTypeIndex = index; });
    connect(_ui.DownloaderThreadUIInput, &QComboBox::currentIndexChanged, [=](int index) { Config::DownloaderThreadUIIndex = index; });

    // Update PO Token on text change
    connect(_ui.POTokenInput, &QLineEdit::textChanged, [=](QString text) { Config::POToken = text; });

    // Button Clicks (Using isChecked to help with loading settings)
    connect(_ui.OverwriteSettingButton, &CheckBox::clicked, [=] { Config::Overwrite = _ui.OverwriteSettingButton->isChecked; });
    connect(_ui.NotificationSettingButton, &CheckBox::clicked, [=] { Config::Notifications = _ui.NotificationSettingButton->isChecked; });
    connect(_ui.NormalizeVolumeSettingButton, &CheckBox::clicked, [=] {
        Config::NormalizeAudio = _ui.NormalizeVolumeSettingButton->isChecked;
        _ui.NormalizeVolumeSettingInput->setEnabled(Config::NormalizeAudio);
    });
    connect(_ui.GetLyricsSettingButton, &CheckBox::clicked, [=] { Config::GetLyrics = _ui.GetLyricsSettingButton->isChecked; });
    connect(_ui.AutoBestQualitySettingButton, &CheckBox::clicked, [=] {
        Config::AutomaticBestQuality = _ui.AutoBestQualitySettingButton->isChecked;

        // Disable bitrate input if setting to highest quality
        _ui.AudioBitrateInput->setEnabled(!Config::AutomaticBestQuality && !Codec::Data[Config::Codec].LockedBitrate);

        // Set bitrate input to the highest possible if set to highest qualty
        if (Config::AutomaticBestQuality) {
            Codec::ExtensionData currentCodecData = Codec::Data[Config::Codec];
            _ui.AudioBitrateInput->setValue(Config::YouTubeCookies.isEmpty() ? currentCodecData.BitrateHighQuality : currentCodecData.BitrateHighQualityPremium);
        }
        // Otherwise set back to manual input
        else {
            _ui.AudioBitrateInput->setValue(Config::GetBitrate());
        }
    });
    connect(_ui.CreateLRCFileSettingButton, &CheckBox::clicked, [=] {Config::CreateLRCFile = _ui.CreateLRCFileSettingButton->isChecked; });
    connect(_ui.AutoOpenFolderButton, &CheckBox::clicked, [=] { Config::AutoOpenDownloadFolder = _ui.AutoOpenFolderButton->isChecked; });
    connect(_ui.SidebarIconsColourButton, &CheckBox::clicked, [=] {
        Config::SidebarIconsColour = _ui.SidebarIconsColourButton->isChecked;

        // Set icons
        _ui.DownloadingScreenButton->setIcon(Config::DownloadIcon());
        _ui.ErrorScreenButton->setIcon(_errorUI.count() > 0 ? Config::ErrorIcon() : Config::ErrorIconInactive());
        _ui.NoticesScreenButton->setIcon(_unreadNotices > 0 ? Config::NoticesAvailableIcon() : Config::NoticesIcon());
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
    connect(_ui.EnableNoticesScreenButton, &CheckBox::clicked, [this] {
        Config::EnableNoticesScreen = _ui.EnableNoticesScreenButton->isChecked;

        _ui.NoticesScreenButton->setVisible(Config::EnableNoticesScreen);
        _ui.CheckForNoticesLabel->setVisible(Config::EnableNoticesScreen);
        _ui.CheckForNoticesButton->setVisible(Config::EnableNoticesScreen);
        _ui.InterfaceSettingsScrollAreaFillerFrame->setVisible(!Config::EnableNoticesScreen); // Prevent scroll bar when all settings are shown

        // If started the app with it disabled, setup the notices screen again
        if (Config::EnableNoticesScreen && _noticesUI.empty())
            SetupNoticesScreen();
    });
    connect(_ui.CheckForNoticesButton, &CheckBox::clicked, [&ui = this->_ui] { Config::CheckForNotices = ui.CheckForNoticesButton->isChecked; });

    // Clear Buttons
    connect(_ui.YoutubeCookiesClearButton, &QPushButton::clicked, [=] {
        // Set Cookies Assigned Label
        _ui.YoutubeCookiesAssignedLabel->setText(R"(<span style=" font-size:13pt; color:#ff6464;">No Cookies Assigned</span>)");

        // Set YouTube Cookies
        Config::YouTubeCookies = "";

        UpdateBitrateInput(Config::Codec);
    });

    connect(_ui.POTokenClearButton, &QPushButton::clicked, [=] {
        _ui.POTokenInput->setText("");
        Config::POToken = "";
    });

    connect(_ui.SpotifyAPIClientClearButton, &QPushButton::clicked, [=] {
        _ui.SpotifyClientIDInput->setText("");
    });

    connect(_ui.SpotifyAPISecretClearButton, &QPushButton::clicked, [=] {
        _ui.SpotifyClientSecretInput->setText("");
    });

    // Paste Buttons
    connect(_ui.YoutubeCookiesPasteButton, &QPushButton::clicked, [=] {
        // Get clipboard text
        QClipboard* clipboard = qApp->clipboard();
        QString clipboardText = clipboard->text();

        // Check if cokies are in a valid format
        bool validFormat = true;
        QString firstLine = clipboardText.split("\n")[0];
        if (firstLine != "# HTTP Cookie File" && firstLine != "# Netscape HTTP Cookie File") {
            validFormat = false;
        }
        
        // Set Cookies Assigned Label
        _ui.YoutubeCookiesAssignedLabel->setText(validFormat ?
            R"(<span style=" font-size:13pt; color:#87D76D;">Cookies Assigned</span>)" : // If in valid format
            R"(<span style=" font-size:13pt; color:#ff6464;">Cookies Invalid</span>)"    // If not in valid format
        );

        // Set YouTube Cookies
        Config::YouTubeCookies = clipboardText;

        // Only update bitrate input if valid
        if (validFormat) {
            UpdateBitrateInput(Config::Codec);

            // If using high quality bitrate type, update input to show the highest bitrate
            _ui.AudioBitrateInput->setValue(Codec::Data[Config::Codec].BitrateHighQualityPremium);
        }

    });

    connect(_ui.POTokenPasteButton, &QPushButton::clicked, [=] {
        QClipboard* clipboard = qApp->clipboard();

        _ui.POTokenInput->setText(clipboard->text());
        Config::POToken = clipboard->text();
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
        OpenURL(QUrl("https://github.com/WilliamSchack/Spotify-Downloader?tab=readme-ov-file#usage"), "Help", "YouTube Cookies are not required. They must be in the netscape format. It is used with the PO Token to remove age restrictions and access YouTube Premium quality. Would you like to access the help documentation?");
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

void SpotifyDownloader::SetupNoticesScreen() {
    if (Config::EnableNoticesScreen && Config::CheckForNotices)
        _notices = NoticesManager::GetLatestNotices();

    // If no notices are available, update the contents text to reflect that
    if (_notices.empty()) {
        _ui.NoticesContent->setText(R"(<p align="center"><span style="font-size:28pt;font-weight:700;">No Notices Available</span></p>)");
        return;
    }

    QVBoxLayout* noticesItemsLayout = (QVBoxLayout*)_ui.NoticesSelectScrollAreaWidgetContents->layout();

    // Setup notices
    _unreadNotices = 0;
    for (Notice& notice : _notices) {
        if (!notice.read) _unreadNotices++;

        // Create UI Item
        NoticeItem* noticeItem = new NoticeItem(notice, _objectHoverWatcher);
        noticesItemsLayout->insertWidget(noticesItemsLayout->count() - 1, noticeItem); // Insert before filler widget

        _noticesUI.append(noticeItem);

        connect(noticeItem, &QPushButton::clicked, [this, noticeItem, &notice] {
            // Deselect all notices
            for (NoticeItem* otherNotice : _noticesUI) {
                otherNotice->Deselect();
            }

            if (!notice.read && _unreadNotices > 0) {
                notice.read = true;
                _unreadNotices--;

                // If no more unread notices, update the icon
                if (_unreadNotices == 0)
                    _ui.NoticesScreenButton->setIcon(Config::NoticesIconFilled());
            }

            // Select this notice and update UI
            noticeItem->Select();
            NoticesManager::ReadNotice(notice.id);
            _ui.NoticesContent->setText(QString::fromStdString(notice.content));
        });
    }

    // Update icon if there are unread notices
    if (_unreadNotices > 0)
        _ui.NoticesScreenButton->setIcon(Config::NoticesAvailableIcon());
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

    // Get Lyrics
    if (_ui.GetLyricsSettingButton->isChecked != Config::GetLyrics)
        _ui.GetLyricsSettingButton->click();

    // Audio Bitrate
    UpdateBitrateInput(Config::Codec);
    _ui.AudioBitrateInput->setValue(Config::GetBitrate());

    // Estimated file size
    float estimatedFileSize = Codec::Data[Config::Codec].CalculateFileSize(Config::GetBitrate(), 60);
    QString fileSizeText = QString("%1MB/min").arg(QString::number(estimatedFileSize, 'f', 2));
    _ui.AudioBitrateFileSizeLabel_Value->setText(fileSizeText);

    // Artist Seperator
    _ui.ArtistSeparatorInput->setText(Config::ArtistSeparator);

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
    
    // LRC File
    if (_ui.CreateLRCFileSettingButton->isChecked != Config::CreateLRCFile)
        _ui.CreateLRCFileSettingButton->click();
    _ui.LRCFileNameTagInput->setText(Config::LRCFileNameTag);
    _ui.LRCFileNameInput->setText(Config::LRCFileName);
    
    // Playlist File
    _ui.PlaylistFileTypeInput->setCurrentIndex(Config::PlaylistFileTypeIndex);
    _ui.PlaylistFileNameTagInput->setText(Config::PlaylistFileNameTag);
    _ui.PlaylistFileNameInput->setText(Config::PlaylistFileName);

    // Status Notifications
    if (_ui.NotificationSettingButton->isChecked != Config::Notifications)
        _ui.NotificationSettingButton->click();
    
    // Downloader Threads
    _ui.DownloaderThreadsInput->setValue(Config::ThreadCount);
    
    // Download Speed Limit
    _ui.DownloadSpeedSettingInput->setValue(Config::DownloadSpeed);
    
    // Download Timeout
    _ui.DownloadTimeoutSettingInput->setValue(Config::DownloadTimeout);

    // YouTube Cookies
    _ui.YoutubeCookiesAssignedLabel->setText(Config::YouTubeCookies.isEmpty() ?
        R"(<span style=" font-size:13pt; color:#ff6464;">No Cookies Assigned</span>)" :
        R"(<span style=" font-size:13pt; color:#87D76D;">Cookies Assigned</span>)"
    );

    // Auto Best Quality (Setup after cookies to correctly set bitrate input)
    if (_ui.AutoBestQualitySettingButton->isChecked != Config::AutomaticBestQuality)
        _ui.AutoBestQualitySettingButton->click();
    _ui.AudioBitrateInput->setEnabled(!Config::AutomaticBestQuality && !Codec::Data[Config::Codec].LockedBitrate);

    // PO Token
    _ui.POTokenInput->setText(Config::POToken);

    // Spotify Client ID
    _ui.SpotifyClientIDInput->setText(SpotifyAPI::ClientID);

    // Spotify Client Secret
    _ui.SpotifyClientSecretInput->setText(SpotifyAPI::ClientSecret);

    // Downloader Thread UI
    _ui.DownloaderThreadUIInput->setCurrentIndex(Config::DownloaderThreadUIIndex);

    // Auto Open Download Folder
    if (_ui.AutoOpenFolderButton->isChecked != Config::AutoOpenDownloadFolder)
        _ui.AutoOpenFolderButton->click();

    // Sidebar Icons Colour
    if (_ui.SidebarIconsColourButton->isChecked != Config::SidebarIconsColour)
        _ui.SidebarIconsColourButton->click();

    // Check for updates
    if (_ui.CheckForUpdatesButton->isChecked != Config::CheckForUpdates)
        _ui.CheckForUpdatesButton->click();

    // Notices Screen
    if (_ui.EnableNoticesScreenButton->isChecked != Config::EnableNoticesScreen)
        _ui.EnableNoticesScreenButton->click();
    if (!Config::EnableNoticesScreen) {
        _ui.NoticesScreenButton->hide();
        _ui.CheckForNoticesLabel->hide();
        _ui.CheckForNoticesButton->hide();
    }

    // Check for notices
    if (_ui.CheckForNoticesButton->isChecked != Config::CheckForNotices)
        _ui.CheckForNoticesButton->click();

    // Set icons colour
    _ui.DownloadingScreenButton->setIcon(Config::DownloadIconFilled()); // Set to filled, current screen will be setup, LoadSettingsUI only called on startup
    _ui.NoticesScreenButton->setIcon(_unreadNotices > 0 ? Config::NoticesAvailableIcon() : Config::NoticesIcon());
    _ui.SettingsScreenButton->setIcon(Config::SettingsIcon());
    _ui.DonateButton->setIcon(Config::DonateIcon());
    _ui.UpdateImageLabel->setPixmap(Config::UpdateIcon());
    _ui.SubmitBugButton->setIcon(Config::BugIcon());
    _ui.HelpButton->setIcon(Config::HelpIcon());
}

bool SpotifyDownloader::ValidateSettings() {
    QStringList namingTags = Config::NAMING_TAGS;

    // Output Format
    std::tuple<QString, Config::NamingError> formattedOutputName = Config::ValidateTagsInString(Config::FileNameTag, Config::FileName, Config::NAMING_TAGS);
    QString formattedOutputNameString = std::get<0>(formattedOutputName);
    Config::NamingError outputNameNamingError = std::get<1>(formattedOutputName);

    switch (outputNameNamingError) {
        case Config::NamingError::EnclosingTagsInvalid:
            ShowMessageBox(
                "Invalid Naming Format Tag",
                QString("Formatting tag must have 2 characters (Opening, Closing)\n%1 is invalid.").arg(formattedOutputNameString),
                QMessageBox::Warning
            );
            return false;
        case Config::NamingError::TagInvalid:
            ShowMessageBox(
                "Invalid Naming Format",
                QString("Invalid Tag Detected:\n%1").arg(formattedOutputNameString),
                QMessageBox::Warning
            );
            return false;
    }

    // Sub Folders

    // Check path for errors
    FileUtils::FilePathError subFolderPathError = FileUtils::CheckInputtedFilePathErrors(Config::SubFolders);
    switch (subFolderPathError) {
        case FileUtils::FilePathError::StartsWithDirectory:
            ShowMessageBox(
                "Invalid Sub Folders",
                R"(Sub Folders cannot start with "/" or "\")",
                QMessageBox::Warning
            );
            return false;
        case FileUtils::FilePathError::EndsWithDirectory:
            ShowMessageBox(
                "Invalid Sub Folders",
                R"(Sub Folders cannot end with "/" or "\")",
                QMessageBox::Warning
            );
            return false;
        case FileUtils::FilePathError::ContainsDoubleSlashes:
            ShowMessageBox(
                "Invalid Sub Folders",
                R"(Sub Folders cannot contain "//" or "\\")",
                QMessageBox::Warning
            );
            return false;
        case FileUtils::FilePathError::InvalidSlashes:
            ShowMessageBox(
                "Invalid Sub Folders",
                R"(Sub Folders cannot contain "/\" or "\/")",
                QMessageBox::Warning
            );
            return false;
    }

    // Check tags
    std::tuple<QString, Config::NamingError> formattedSubFolders = Config::ValidateTagsInString(Config::SubFoldersTag, Config::SubFolders, Config::NAMING_TAGS);
    QString formattedSubFoldersString = std::get<0>(formattedSubFolders);
    Config::NamingError subFoldersNamingError = std::get<1>(formattedSubFolders);

    switch (subFoldersNamingError) {
        case Config::NamingError::EnclosingTagsInvalid:
            ShowMessageBox(
                "Invalid Sub Folders Tag",
                QString("Formatting tag must have 2 characters (Opening, Closing)\n%1 is invalid.").arg(formattedSubFoldersString),
                QMessageBox::Warning
            );
            return false;
        case Config::NamingError::TagInvalid:
            ShowMessageBox(
                "Invalid Sub Folders",
                QString("Invalid Tag Detected:\n%1").arg(formattedSubFoldersString),
                QMessageBox::Warning
            );
            return false;
    }

    // LRC File

    // Check path for errors
    FileUtils::FilePathError lrcFilePathError = FileUtils::CheckInputtedFilePathErrors(Config::LRCFileName);
    switch (lrcFilePathError) {
    case FileUtils::FilePathError::StartsWithDirectory:
        ShowMessageBox(
            "Invalid LRC File Name",
            R"(LRC File Name cannot start with "/" or "\")",
            QMessageBox::Warning
        );
        return false;
    case FileUtils::FilePathError::EndsWithDirectory:
        ShowMessageBox(
            "Invalid LRC File Name",
            R"(LRC File Name cannot end with "/" or "\")",
            QMessageBox::Warning
        );
        return false;
    case FileUtils::FilePathError::ContainsDoubleSlashes:
        ShowMessageBox(
            "Invalid LRC File Name",
            R"(LRC File Name cannot contain "//" or "\\")",
            QMessageBox::Warning
        );
        return false;
    case FileUtils::FilePathError::InvalidSlashes:
        ShowMessageBox(
            "Invalid LRC File Name",
            R"(LRC File Name cannot contain "/\" or "\/")",
            QMessageBox::Warning
        );
        return false;
    }

    // Check tags
    QStringList validLRCFileNameTags = Config::NAMING_TAGS;
    validLRCFileNameTags.append(Config::SAVE_LOCATION_TAG);
    std::tuple<QString, Config::NamingError> formattedLRCFileName = Config::ValidateTagsInString(Config::LRCFileNameTag, Config::LRCFileName, validLRCFileNameTags);
    QString formattedLRCFileNameString = std::get<0>(formattedLRCFileName);
    Config::NamingError LRCFileNameNamingError = std::get<1>(formattedLRCFileName);

    switch (LRCFileNameNamingError) {
        case Config::NamingError::EnclosingTagsInvalid:
            ShowMessageBox(
                "Invalid Playlist File Name Tag",
                QString("Formatting tag must have 2 characters (Opening, Closing)\n%1 is invalid.").arg(formattedLRCFileNameString),
                QMessageBox::Warning
            );
            return false;
        case Config::NamingError::TagInvalid:
            ShowMessageBox(
                "Invalid Playlist File Name",
                QString("Invalid Tag Detected:\n%1").arg(formattedLRCFileNameString),
                QMessageBox::Warning
            );
            return false;
    }

    // Playlist File

    // Check path for errors
    FileUtils::FilePathError playlistFilePathError = FileUtils::CheckInputtedFilePathErrors(Config::PlaylistFileName);
    switch (playlistFilePathError) {
        case FileUtils::FilePathError::StartsWithDirectory:
            ShowMessageBox(
                "Invalid Playlist File Name",
                R"(Playlist File Name cannot start with "/" or "\")",
                QMessageBox::Warning
            );
            return false;
        case FileUtils::FilePathError::EndsWithDirectory:
            ShowMessageBox(
                "Invalid Playlist File Name",
                R"(Playlist File Name cannot end with "/" or "\")",
                QMessageBox::Warning
            );
            return false;
        case FileUtils::FilePathError::ContainsDoubleSlashes:
            ShowMessageBox(
                "Invalid Playlist File Name",
                R"(Playlist File Name cannot contain "//" or "\\")",
                QMessageBox::Warning
            );
            return false;
        case FileUtils::FilePathError::InvalidSlashes:
            ShowMessageBox(
                "Invalid Playlist File Name",
                R"(Playlist File Name cannot contain "/\" or "\/")",
                QMessageBox::Warning
            );
            return false;
    }

    // Check tags
    QStringList validPlaylistFileNameTags = Config::PLAYLIST_NAMING_TAGS;
    validPlaylistFileNameTags.append(Config::SAVE_LOCATION_TAG);
    std::tuple<QString, Config::NamingError> formattedPlaylistFileName = Config::ValidateTagsInString(Config::PlaylistFileNameTag, Config::PlaylistFileName, validPlaylistFileNameTags);
    QString formattedPlaylistFileNameString = std::get<0>(formattedPlaylistFileName);
    Config::NamingError PlaylistFileNameNamingError = std::get<1>(formattedPlaylistFileName);

    switch (PlaylistFileNameNamingError) {
        case Config::NamingError::EnclosingTagsInvalid:
            ShowMessageBox(
                "Invalid Playlist File Name Tag",
                QString("Formatting tag must have 2 characters (Opening, Closing)\n%1 is invalid.").arg(formattedPlaylistFileNameString),
                QMessageBox::Warning
            );
            return false;
        case Config::NamingError::TagInvalid:
            ShowMessageBox(
                "Invalid Playlist File Name",
                QString("Invalid Tag Detected:\n%1").arg(formattedPlaylistFileNameString),
                QMessageBox::Warning
            );
            return false;
    }

    // Youtube Cookies / PO Token, return false if one is set without the other
    if ((!Config::YouTubeCookies.isEmpty() && Config::POToken.isEmpty()) || (Config::YouTubeCookies.isEmpty() && !Config::POToken.isEmpty())) {
        ShowMessageBox(
            "Invalid Youtube Cookies",
            "Both the Youtube Cookies and PO Token must be set",
            QMessageBox::Warning
        );
        return false;
    }

    // Youtube cookies must be in netscape format
    if (!Config::YouTubeCookies.isEmpty()) {
        QString firstLine = Config::YouTubeCookies.split("\n")[0];
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
    if (!std::filesystem::exists(FileUtils::ToNativeFilePath(Config::SaveLocation))) {
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