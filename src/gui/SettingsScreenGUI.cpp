#include "SettingsScreenGUI.h"

SettingsScreenGUI::SettingsScreenGUI(QObject* parent) : QObject(parent)
{
    connect(this, &SettingsScreenGUI::FileNameTagsOpeningChanged, this, &SettingsScreenGUI::FileNameValidChanged);
    connect(this, &SettingsScreenGUI::FileNameTagsClosingChanged, this, &SettingsScreenGUI::FileNameValidChanged);
    connect(this, &SettingsScreenGUI::FileNameChanged, this, &SettingsScreenGUI::FileNameValidChanged);

    connect(this, &SettingsScreenGUI::SubFoldersTagsOpeningChanged, this, &SettingsScreenGUI::SubFoldersValidChanged);
    connect(this, &SettingsScreenGUI::SubFoldersTagsClosingChanged, this, &SettingsScreenGUI::SubFoldersValidChanged);
    connect(this, &SettingsScreenGUI::SubFoldersChanged, this, &SettingsScreenGUI::SubFoldersValidChanged);

    connect(this, &SettingsScreenGUI::LyricsFileNameTagsOpeningChanged, this, &SettingsScreenGUI::LyricsFileNameValidChanged);
    connect(this, &SettingsScreenGUI::LyricsFileNameTagsClosingChanged, this, &SettingsScreenGUI::LyricsFileNameValidChanged);
    connect(this, &SettingsScreenGUI::LyricsFileNameChanged, this, &SettingsScreenGUI::LyricsFileNameValidChanged);

    _fileNameOpeningChar = Config::FileNameTagsOpeningChar;
    _fileNameClosingChar = Config::FileNameTagsClosingChar;
    _fileName = Config::FileName;

    _subFoldersOpeningChar = Config::SubFoldersTagsOpeningChar;
    _subFoldersClosingChar = Config::SubFoldersTagsClosingChar;
    _subFolders = Config::SubFolders;

    _lyricsFileNameOpeningChar = Config::LyricsFileNameTagsOpeningChar;
    _lyricsFileNameClosingChar = Config::LyricsFileNameTagsClosingChar;
    _lyricsFileName = Config::LyricsFileName;
}

bool SettingsScreenGUI::GetFileNameValid()
{
    TrackTagHandler tagHandler(TrackData(EPlatform::Unknown));
    TagHandlerResult result = tagHandler.FormatString(_fileName, _fileNameOpeningChar, _fileNameClosingChar);
    _fileNameValid = result.Error == ETagError::None;
    
    return _fileNameValid;
}

bool SettingsScreenGUI::GetSubFoldersValid()
{
    TrackTagHandler tagHandler(TrackData(EPlatform::Unknown));
    TagHandlerResult result = tagHandler.FormatString(_subFolders, _subFoldersOpeningChar, _subFoldersClosingChar);
    _subFoldersValid = result.Error == ETagError::None;

    return _subFoldersValid;
}

bool SettingsScreenGUI::GetLyricsFileNameValid()
{
    SavableTrackTagHandler tagHandler(TrackData(EPlatform::Unknown), "");
    TagHandlerResult result = tagHandler.FormatString(_lyricsFileName, _lyricsFileNameOpeningChar, _lyricsFileNameClosingChar);
    _lyricsFileNameValid = result.Error == ETagError::None;

    return _lyricsFileNameValid;
}

bool SettingsScreenGUI::GetAllSettingsValid() const
{
    return _fileNameValid && _subFoldersValid && _lyricsFileNameValid;
}

void SettingsScreenGUI::LeavingScreen()
{
    Config::FileNameTagsOpeningChar = _fileNameOpeningChar;
    Config::FileNameTagsClosingChar = _fileNameClosingChar;
    Config::FileName = _fileName;

    Config::SubFoldersTagsOpeningChar = _subFoldersOpeningChar;
    Config::SubFoldersTagsClosingChar = _subFoldersClosingChar;
    Config::SubFolders = _subFolders;

    Config::LyricsFileNameTagsOpeningChar = _lyricsFileNameOpeningChar;
    Config::LyricsFileNameTagsClosingChar = _lyricsFileNameClosingChar;
    Config::LyricsFileName = _lyricsFileName;
}