#include "GUIManager.h"

GUIManager::GUIManager(QObject* parent) : QObject(parent)
{
    _musicManager = new MusicScreenGUI(this);
    _settingsManager = new SettingsScreenGUI(this);

    SetupTrayIcon();

    connect(_musicManager, &MusicScreenGUI::RequestNotification, this, &GUIManager::SendNotification);
}

MusicScreenGUI* GUIManager::GetMusicManager() const
{
    return _musicManager;
}

SettingsScreenGUI* GUIManager::GetSettingsManager() const
{
    return _settingsManager;
}

void GUIManager::SetupTrayIcon()
{
    QAction* quitAction = new QAction("Quit");
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

    QMenu* trayMenu = new QMenu();
    trayMenu->addAction(quitAction);

    _trayIcon.setIcon(QIcon(":/icons/Icon.png"));
    _trayIcon.setContextMenu(trayMenu);
    _trayIcon.show();
}

void GUIManager::SendNotification(const std::string& message)
{
    _trayIcon.showMessage("Echolite", QString::fromStdString(message), _trayIcon.icon(), NOTIFICATION_MS);
}