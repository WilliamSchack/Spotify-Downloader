#ifndef CONFIG_H
#define CONFIG_H

#define QT_MESSAGELOGCONTEXT
#include "Utilities/Logger.h"

#include "Downloading/Codec.h"

#include <QSettings>
#include <QJsonObject>

class Config {
    public:
        static constexpr const char* VERSION = "1.5.1";
        static constexpr const char* ORGANIZATION_NAME = "WilliamSchack";
        static constexpr const char* APPLICATION_NAME = "Spotify Downloader";

        static constexpr const char* YTDLP_PATH = "yt-dlp.exe";
        static constexpr const char* FFMPEG_PATH = "ffmpeg.exe";

        static const int SETUP_SCREEN_INDEX = 0;
        static const int SETTINGS_SCREEN_INDEX = 1;
        static const int PROCESSING_SCREEN_INDEX = 2;
        static const int ERROR_SCREEN_INDEX = 3;

        static const int OUTPUT_SETTINGS_LINE_MAX_HEIGHT = 255;
        static const int DOWNLOADING_SETTINGS_LINE_MAX_HEIGHT = 105;
        static const int INTERFACE_SETTINGS_LINE_MAX_HEIGHT = 5;

        static constexpr const char* NAMING_TAGS[] = {
            "song name",
            "album name",
            "song artist",
            "song artists",
            "album artist",
            "album artists",
            "song time seconds",
            "song time sinutes",
            "song time hours"
        };

        enum class NamingError {
            None,
            EnclosingTagsInvalid,
            TagInvalid
        };

        static inline Codec::Extension Codec; // TEMP

        // Output
        static inline bool Overwrite;
    
        static inline bool NormalizeAudio;
        static inline float NormalizeAudioVolume;
    
        static inline int AudioBitrate;

        static inline QString PlaylistURL;
        static inline QString SaveLocation;
    
        static inline QString SongOutputFormatTag;
        static inline QString SongOutputFormat;
    
        static inline int FolderSortingIndex;
    
        // Downloading
        static inline bool Notifications;
    
        static inline int ThreadCount;
        static inline float DownloadSpeed;
    
        static QStringList Q_NAMING_TAGS();
        static std::tuple<QString, NamingError> FormatOutputNameWithTags(std::function<QString(QString)> tagHandlerFunc); // Output, Error
    
        // Interface
        static inline int DownloaderThreadUIIndex;

        static void SaveSettings();
        static void LoadSettings();
    private:
        static inline QStringList Q_NAMING_TAGS_CACHE;
};

#endif