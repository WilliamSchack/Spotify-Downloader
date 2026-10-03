#include "GUIManager.h"

GUIManager::GUIManager(QObject* parent) : QObject(parent)
{
    QQuickStyle::setStyle("Basic");

    _musicManager = new MusicScreenGUI(this);
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