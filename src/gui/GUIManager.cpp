#include "GUIManager.h"

GUIManager::GUIManager(QObject* parent) : QObject(parent)
{
    _musicManager = new MusicScreenGUI(this);
    _infoManager = new InfoScreenGUI(this);
    _settingsManager = new SettingsScreenGUI(this);

    SetupTrayIcon();
    SetupColours();

    connect(_musicManager, &MusicScreenGUI::RequestNotification, this, &GUIManager::SendNotification);
}

MusicScreenGUI* GUIManager::GetMusicManager() const
{
    return _musicManager;
}

InfoScreenGUI* GUIManager::GetInfoManager() const
{
    return _infoManager;
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

void GUIManager::SetupColours()
{
    // Cant set the link colour for markdown text in qml, do it here instead

    QPalette palette = qApp->palette();
    palette.setColor(QPalette::Link, QColor("#4ec1ff"));
    qApp->setPalette(palette);
}

void GUIManager::SendNotification(const std::string& message)
{
    if (!Config::ShowStatusNotifications)
        return;

    _trayIcon.showMessage("Echolite", QString::fromStdString(message), _trayIcon.icon(), NOTIFICATION_MS);
}