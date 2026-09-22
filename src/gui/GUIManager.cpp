#include "GUIManager.h"

GUIManager::GUIManager(QObject* parent) : QObject(parent)
{
    _musicManager = new MusicScreenGUI(this);
}

MusicScreenGUI* GUIManager::GetMusicManager() const
{
    return _musicManager;
}