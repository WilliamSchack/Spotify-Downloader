#ifndef SETTINGSSCREENGUI_H
#define SETTINGSSCREENGUI_H

#include "EasyQProperties.h"
#include "Config.h"
#include "SavableTrackTagHandler.h"

#include <QObject>

class SettingsScreenGUI : public QObject
{
    Q_OBJECT
    ENUM_QPROPERTY(EExtension, EEXTENSION_NAMES, codec, Codec, Config::CodecExtension)
    SIMPLE_QPROPERTY(bool, normalise, Normalise, Config::Normalise)
    SIMPLE_QPROPERTY(double, normaliseDb, NormaliseDb, Config::NormaliseDb)
    SIMPLE_QPROPERTY(bool, manualBitrate, ManualBitrate, Config::ManualBitrate)
    SIMPLE_QPROPERTY(int, bitrate, Bitrate, Config::BitrateKbps)
    
    SIMPLE_QPROPERTY(bool, getLyrics, GetLyrics, Config::GetLyrics)
    STRING_QPROPERTY(artistsSeparator, ArtistsSeparator, Config::ArtistSeperator)
    ENUM_QPROPERTY(ETrackNumberType, ETRACKNUMBERTYPE_NAMES, trackNumberType, TrackNumberType, Config::TrackNumberType)
    
    SIMPLE_QPROPERTY(bool, overwrite, Overwrite, Config::Overwrite)
    CHAR_QPROPERTY(fileNameTagsOpening, FileNameTagsOpening, _fileNameOpeningChar)
    CHAR_QPROPERTY(fileNameTagsClosing, FileNameTagsClosing, _fileNameClosingChar)
    STRING_QPROPERTY(fileName, FileName, _fileName)
    Q_PROPERTY(bool fileNameValid READ GetFileNameValid NOTIFY FileNameValidChanged)
    CHAR_QPROPERTY(subFoldersTagsOpening, SubFoldersTagsOpening, _subFoldersOpeningChar)
    CHAR_QPROPERTY(subFoldersTagsClosing, SubFoldersTagsClosing, _subFoldersClosingChar)
    STRING_QPROPERTY(subFolders, SubFolders, _subFolders)
    Q_PROPERTY(bool subFoldersValid READ GetSubFoldersValid NOTIFY SubFoldersValidChanged)

    SIMPLE_QPROPERTY(bool, createLyricsFile, CreateLyricsFile, Config::CreateLyricsFile)
    CHAR_QPROPERTY(lyricsFileNameTagsOpening, LyricsFileNameTagsOpening, _lyricsFileNameOpeningChar)
    CHAR_QPROPERTY(lyricsFileNameTagsClosing, LyricsFileNameTagsClosing, _lyricsFileNameClosingChar)
    STRING_QPROPERTY(lyricsFileName, LyricsFileName, _lyricsFileName)
    Q_PROPERTY(bool lyricsFileNameValid READ GetLyricsFileNameValid NOTIFY LyricsFileNameValidChanged)

    SIMPLE_QPROPERTY(int, downloadThreads, DownloadThreads, Config::PerDownloadThreads)
    SIMPLE_QPROPERTY(double, downloadSpeedLimit, DownloadSpeedLimit, Config::DownloadSpeedLimit)
    ENUM_QPROPERTY(EIPVersion, EIPVERSION_NAMES, ipVersion, IPVersion, Config::IPVersion)
    
    SIMPLE_QPROPERTY(bool, autoOpenDownloadFolder, AutoOpenDownloadFolder, Config::AutoOpenDownloadFolder)

    Q_PROPERTY(bool allSettingsValid READ GetAllSettingsValid)
    
    public:
        explicit SettingsScreenGUI(QObject* parent = 0);

        bool GetFileNameValid();
        bool GetSubFoldersValid();
        bool GetLyricsFileNameValid();
        bool GetAllSettingsValid() const;

        Q_INVOKABLE void LeavingScreen();
    private:
        char _fileNameOpeningChar;
        char _fileNameClosingChar;
        std::string _fileName;
        bool _fileNameValid;

        char _subFoldersOpeningChar;
        char _subFoldersClosingChar;
        std::string _subFolders;
        bool _subFoldersValid;

        char _lyricsFileNameOpeningChar;
        char _lyricsFileNameClosingChar;
        std::string _lyricsFileName;
        bool _lyricsFileNameValid;
    signals:
        void FileNameValidChanged();
        void SubFoldersValidChanged();
        void LyricsFileNameValidChanged();
};

#endif