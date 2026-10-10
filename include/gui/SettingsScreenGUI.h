#ifndef SETTINGSSCREENGUI_H
#define SETTINGSSCREENGUI_H

#include "Config.h"
#include "EasyQProperties.h"

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
    CHAR_QPROPERTY(fileNameTagsOpening, FileNameTagsOpening, Config::FileNameTagsOpeningChar)
    CHAR_QPROPERTY(fileNameTagsClosing, FileNameTagsClosing, Config::FileNameTagsClosingChar)
    STRING_QPROPERTY(fileName, FileName, Config::FileName)
    CHAR_QPROPERTY(subFoldersTagsOpening, SubFoldersTagsOpening, Config::SubFoldersTagsOpeningChar)
    CHAR_QPROPERTY(subFoldersTagsClosing, SubFoldersTagsClosing, Config::SubFoldersTagsClosingChar)
    STRING_QPROPERTY(subFolders, SubFolders, Config::SubFolders)
    SIMPLE_QPROPERTY(int, downloadThreads, DownloadThreads, Config::PerDownloadThreads)
    SIMPLE_QPROPERTY(double, downloadSpeedLimit, DownloadSpeedLimit, Config::DownloadSpeedLimit)
    ENUM_QPROPERTY(EIPVersion, EIPVERSION_NAMES, ipVersion, IPVersion, Config::IPVersion)
    SIMPLE_QPROPERTY(bool, autoOpenDownloadFolder, AutoOpenDownloadFolder, Config::AutoOpenDownloadFolder)

    public:
        explicit SettingsScreenGUI(QObject* parent = 0);
};

#endif