#include "GUIManager.h"

GUIManager::GUIManager(QWindow* mainWindow, QObject* parent) : _mainWindow(mainWindow), QObject(parent)
{
    QQuickStyle::setStyle("Basic");

    _musicManager = new MusicScreenGUI(mainWindow, this);
    _settingsManager = new SettingsScreenGUI(this);
}

MusicScreenGUI* GUIManager::GetMusicManager() const
{
    return _musicManager;
}

SettingsScreenGUI* GUIManager::GetSettingsManager() const
{
    return _settingsManager;
}