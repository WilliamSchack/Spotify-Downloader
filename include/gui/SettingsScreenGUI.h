#ifndef SETTINGSSCREENGUI_H
#define SETTINGSSCREENGUI_H

#include "Config.h"
#include "EasyQProperties.h"

#include <QObject>

class SettingsScreenGUI : public QObject
{
    Q_OBJECT
    ENUM_QPROPERTY(EExtension, EEXTENSION_NAMES, codec, Codec, Config::CODEC_EXTENSION)
    SIMPLE_QPROPERTY(bool, overwrite, Overwrite, Config::OVERWRITE)
    SIMPLE_QPROPERTY(bool, normalise, Normalise, Config::NORMALISE)
    SIMPLE_QPROPERTY(bool, getLyrics, GetLyrics, Config::GET_LYRICS)
    SIMPLE_QPROPERTY(float, normaliseDb, NormaliseDb, Config::NORMALISE_DB)
    SIMPLE_QPROPERTY(bool, manualBitrate, ManualBitrate, Config::MANUAL_BITRATE)
    SIMPLE_QPROPERTY(int, bitrate, Bitrate, Config::BITRATE)
    STRING_QPROPERTY(artistsSeperator, ArtistsSeperator, Config::ARTISTS_SEPERATOR)
    SIMPLE_QPROPERTY(int, downloadThreads, DownloadThreads, Config::PER_DOWNLOAD_THREADS)

    public:
        explicit SettingsScreenGUI(QObject* parent = 0);
};

#endif