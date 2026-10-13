#ifndef SETTINGSSCREENGUI_H
#define SETTINGSSCREENGUI_H

#include "EasyQProperties.h"
#include "Config.h"
#include "SavableTrackTagHandler.h"
#include "SavablePlaylistTagHandler.h"

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
    TAG_QPROPERTY(fileName, FileName, FileName, SettingsScreenGUI, TrackTagHandler, TrackData(EPlatform::Unknown))
    TAG_QPROPERTY(subFolders, SubFolders, SubFolders, SettingsScreenGUI, TrackTagHandler, TrackData(EPlatform::Unknown))

    SIMPLE_QPROPERTY(bool, createLyricsFile, CreateLyricsFile, Config::CreateLyricsFile)
    TAG_QPROPERTY(lyricsFileName, LyricsFileName, LyricsFileName, SettingsScreenGUI, SavableTrackTagHandler, TrackData(EPlatform::Unknown))

    SIMPLE_QPROPERTY(bool, createPlaylistFile, CreatePlaylistFile, Config::CreatePlaylistFile)
    ENUM_QPROPERTY(EPlaylistFileType, EPLAYLISTFILETYPE_NAMES, playlistFileType, PlaylistFileType, Config::PlaylistFileType)
    TAG_QPROPERTY(playlistFileName, PlaylistFileName, PlaylistFileName, SettingsScreenGUI, SavablePlaylistTagHandler, PlaylistData(EPlatform::Unknown))

    SIMPLE_QPROPERTY(int, downloadThreads, DownloadThreads, Config::PerDownloadThreads)
    SIMPLE_QPROPERTY(double, downloadSpeedLimit, DownloadSpeedLimit, Config::DownloadSpeedLimit)
    SIMPLE_QPROPERTY(int, downloadTimeoutMs, DownloadTimeoutMs, Config::DownloadTimeoutMs)
    ENUM_QPROPERTY(EIPVersion, EIPVERSION_NAMES, ipVersion, IPVersion, Config::IPVersion)
    
    SIMPLE_QPROPERTY(bool, autoOpenDownloadFolder, AutoOpenDownloadFolder, Config::AutoOpenDownloadFolder)

    Q_PROPERTY(bool allSettingsValid READ GetAllSettingsValid)
    
    public:
        explicit SettingsScreenGUI(QObject* parent = 0);

        bool GetAllSettingsValid() const;

        Q_INVOKABLE void LeavingScreen();
};

#endif