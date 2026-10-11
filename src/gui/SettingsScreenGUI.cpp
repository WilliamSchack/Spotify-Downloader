#include "SettingsScreenGUI.h"

SettingsScreenGUI::SettingsScreenGUI(QObject* parent) : QObject(parent)
{
    SetupFileName();
    SetupSubFolders();
    SetupLyricsFileName();
}

bool SettingsScreenGUI::GetAllSettingsValid() const
{
    return _fileNameValid && _subFoldersValid && _lyricsFileNameValid;
}

void SettingsScreenGUI::LeavingScreen()
{
    SaveFileName();
    SaveSubFolders();
    SaveLyricsFileName();
}