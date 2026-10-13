#include "SettingsScreenGUI.h"

SettingsScreenGUI::SettingsScreenGUI(QObject* parent) : QObject(parent)
{
    SetupFileName();
    SetupSubFolders();
    SetupLyricsFileName();
    SetupPlaylistFileName();
}

bool SettingsScreenGUI::GetAllSettingsValid() const
{
    return _fileNameValid && _subFoldersValid && _lyricsFileNameValid && _playlistFileNameValid;
}

void SettingsScreenGUI::LeavingScreen()
{
    SaveFileName();
    SaveSubFolders();
    SaveLyricsFileName();
    SavePlaylistFileName();
}