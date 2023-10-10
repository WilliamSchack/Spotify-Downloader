#include "SpotifyDownloader.h"

#include "CustomWidgets.h"

// Ui Setup
SpotifyDownloader::SpotifyDownloader(QWidget* parent) : QDialog(parent)
{
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);

    _ui.setupUi(this);

    SetupTrayIcon();
    SetupSetupScreen();
    SetupSettingsScreen();
    SetupProcessingScreen();

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
                message = "0.000% Complete";
            else
                message = QString("%1% Complete").arg(std::round(((completed * 1.0) / (total * 1.0)) / 0.001) * 0.001);

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

        if (reply == QMessageBox::Yes) {
            closeEvent->accept();
        }
        else closeEvent->ignore();
        return;
    }

    closeEvent->accept();
}

void SpotifyDownloader::SetupDownloaderThread() {

    //// Get thread ready to be started
    //_songDownloader = new SongDownloader();
    //_songDownloader->moveToThread(&workerThread);
    //connect(&workerThread, &QThread::finished, _songDownloader, &QObject::deleteLater);
    //connect(this, &SpotifyDownloader::operate, _songDownloader, &SongDownloader::DownloadSongs);
    //
    //// Allow thread to access ui elements
    //connect(_songDownloader, &SongDownloader::ChangeScreen, this, &SpotifyDownloader::ChangeScreen);
    //connect(_songDownloader, &SongDownloader::ShowMessage, this, &SpotifyDownloader::ShowMessage);
    //connect(_songDownloader, &SongDownloader::SetProgressLabel, this, &SpotifyDownloader::SetProgressLabel);
    //connect(_songDownloader, &SongDownloader::SetProgressBar, this, &SpotifyDownloader::SetProgressBar);
    //connect(_songDownloader, &SongDownloader::SetSongCount, this, &SpotifyDownloader::SetSongCount);
    //connect(_songDownloader, &SongDownloader::SetSongImage, this, &SpotifyDownloader::SetSongImage);
    //connect(_songDownloader, &SongDownloader::SetSongDetails, this, &SpotifyDownloader::SetSongDetails);
    //connect(_songDownloader, &SongDownloader::SetErrorItems, this, &SpotifyDownloader::SetErrorItems);
    //connect(_songDownloader, &SongDownloader::HidePauseWarning, this, &SpotifyDownloader::HidePauseWarning);

    // Get thread ready to be started
    _songDownloader = new PlaylistDownloader();
    _songDownloader->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, _songDownloader, &QObject::deleteLater);
    connect(this, &SpotifyDownloader::operate, _songDownloader, &PlaylistDownloader::DownloadSongs);

    // Allow thread to access ui elements
    connect(_songDownloader, &PlaylistDownloader::ChangeScreen, this, &SpotifyDownloader::ChangeScreen);
    connect(_songDownloader, &PlaylistDownloader::ShowMessage, this, &SpotifyDownloader::ShowMessage);
    connect(_songDownloader, &PlaylistDownloader::SetProgressLabel, this, &SpotifyDownloader::SetProgressLabel);
    connect(_songDownloader, &PlaylistDownloader::SetProgressBar, this, &SpotifyDownloader::SetProgressBar);
    connect(_songDownloader, &PlaylistDownloader::SetSongCount, this, &SpotifyDownloader::SetSongCount);
    connect(_songDownloader, &PlaylistDownloader::SetSongImage, this, &SpotifyDownloader::SetSongImage);
    connect(_songDownloader, &PlaylistDownloader::SetSongDetails, this, &SpotifyDownloader::SetSongDetails);
    connect(_songDownloader, &PlaylistDownloader::SetErrorItems, this, &SpotifyDownloader::SetErrorItems);
    connect(_songDownloader, &PlaylistDownloader::HidePauseWarning, this, &SpotifyDownloader::HidePauseWarning);
}

// Application Exit
SpotifyDownloader::~SpotifyDownloader()
{
    _trayIcon->hide();
    
    workerThread.requestInterruption();
    workerThread.wait();
}