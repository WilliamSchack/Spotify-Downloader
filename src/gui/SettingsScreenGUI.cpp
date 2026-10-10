#include "SettingsScreenGUI.h"

SettingsScreenGUI::SettingsScreenGUI(QObject* parent) : QObject(parent)
{
    connect(this, &SettingsScreenGUI::FileNameTagsOpeningChanged, this, &SettingsScreenGUI::FileNameValidChanged);
    connect(this, &SettingsScreenGUI::FileNameTagsClosingChanged, this, &SettingsScreenGUI::FileNameValidChanged);
    connect(this, &SettingsScreenGUI::FileNameChanged, this, &SettingsScreenGUI::FileNameValidChanged);

    _fileName = Config::FileName;
}

bool SettingsScreenGUI::GetFileNameValid()
{
    TrackTagHandler tagHandler(TrackData(EPlatform::Unknown));
    TagHandlerResult result = tagHandler.FormatString(_fileName, Config::FileNameTagsOpeningChar, Config::FileNameTagsClosingChar);
    _fileNameValid = result.Error == ETagError::None;
    
    return _fileNameValid;
}

bool SettingsScreenGUI::GetAllSettingsValid() const
{
    return _fileNameValid;
}

void SettingsScreenGUI::LeavingScreen()
{
    Config::FileName = _fileName;
}