#include "SpotifyDownloader.h"

// Ui Setup
SpotifyDownloader::SpotifyDownloader(QWidget* parent) : QDialog(parent)
{
    _ui.setupUi(this);

    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);

    // Get thread ready to be started
    SongDownloader *songDownloader = new SongDownloader();
    songDownloader->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, songDownloader, &QObject::deleteLater);
    connect(this, &SpotifyDownloader::operate, songDownloader, &SongDownloader::DownloadSongs);

    // Setup Tray Icon
    QAction* progressAction = new QAction(tr("Current Progress"));
    connect(progressAction, &QAction::triggered, this, [&] {
        if (DownloadStarted) {
            int completed = _songsCompleted;
            int total = _totalSongs;
            ShowMessage(QString("%1 Complete").arg(completed / total),
                QString("Completed: %1/%2").arg(completed).arg(total));
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

    // Allow thread to access ui elements
    connect(songDownloader, &SongDownloader::ChangeScreen, this, &SpotifyDownloader::ChangeScreen);
    connect(songDownloader, &SongDownloader::ShowMessage, this, &SpotifyDownloader::ShowMessage);
    connect(songDownloader, &SongDownloader::SetProgressLabel, this, &SpotifyDownloader::SetProgressLabel);
    connect(songDownloader, &SongDownloader::SetProgressBar, this, &SpotifyDownloader::SetProgressBar);
    connect(songDownloader, &SongDownloader::SetSongCount, this, &SpotifyDownloader::SetSongCount);
    connect(songDownloader, &SongDownloader::SetSongImage, this, &SpotifyDownloader::SetSongImage);
    connect(songDownloader, &SongDownloader::SetSongDetails, this, &SpotifyDownloader::SetSongDetails);
    connect(songDownloader, &SongDownloader::SetErrorItems, this, &SpotifyDownloader::SetErrorItems);
    connect(songDownloader, &SongDownloader::HidePauseWarning, this, &SpotifyDownloader::HidePauseWarning);
    
    // Setup UI Elements
    SetupSetupScreen();
    SetupSettingsScreen();
    SetupProcessingScreen();
}

void SpotifyDownloader::SetupSetupScreen() {
    connect(_ui.SettingsButton, &QPushButton::clicked, [=] {
        _ui.Screens->setCurrentIndex(4); // Change to settings screen
    });

    connect(_ui.BrowseButton, &QPushButton::clicked, [=] {
        QString directory = QFileDialog::getExistingDirectory(this, tr("Choose Save Location"),"",QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if(directory != "") _ui.SaveLocationInput->setPlainText(directory);
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

            DownloadStarted = true;

            _ui.Screens->setCurrentIndex(1);

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
    // Check Boxes
    _ui.NotificationSettingButton->isChecked = true;
    _ui.NormalizeVolumeSettingButton->isChecked = true;

    NormalizeAudioVolume = (_ui.NormalizeVolumeSettingInput->text()).toFloat();

    // Number Inputs
    connect(_ui.DownloadSpeedSettingInput, &QSpinBox::textChanged, [=] {
        if (_ui.DownloadSpeedSettingInput->text() != "") {
            DownloadSpeed = (_ui.DownloadSpeedSettingInput->text()).toFloat();
        }
    });
    connect(_ui.NormalizeVolumeSettingInput, &QDoubleSpinBox::textChanged, [=] {
        if (_ui.NormalizeVolumeSettingInput->text() != "") {
            NormalizeAudioVolume = (_ui.NormalizeVolumeSettingInput->text()).toFloat();
        }
    });

    // Button Clicks
    connect(_ui.OverwriteSettingButton, &CheckBox::clicked, [=] { Overwrite = !Overwrite; });
    connect(_ui.NotificationSettingButton, &CheckBox::clicked, [=] { Notifications = !Notifications; });
    connect(_ui.NormalizeVolumeSettingButton, &CheckBox::clicked, [=] {
        NormalizeAudio = !NormalizeAudio;
        _ui.NormalizeVolumeSettingInput->setEnabled(NormalizeAudio);
    });
    connect(_ui.BackButton, &QPushButton::clicked, [=] {
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

        if (reply == QMessageBox::Yes) closeEvent->accept();
        else closeEvent->ignore();
        return;
    }

    closeEvent->accept();
}

// Application Exit
SpotifyDownloader::~SpotifyDownloader()
{
    workerThread.quit();
    workerThread.wait();
}