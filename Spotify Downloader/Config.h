#ifndef CONFIG_H
#define CONFIG_H

#define QT_MESSAGELOGCONTEXT
#include "Utilities/Logger.h"

#include "Downloading/Codec.h"

#include <QSettings>
#include <QJsonObject>
#include <QMap>
#include <QIcon>

class Config {
    public:
        enum class NamingError {
            None,
            EnclosingTagsInvalid,
            TagInvalid
        };

        static constexpr const char* VERSION = "1.6.0";
        static constexpr const char* ORGANIZATION_NAME = "WilliamSchack";
        static constexpr const char* APPLICATION_NAME = "Spotify Downloader";

        static constexpr const char* YTDLP_PATH = "yt-dlp.exe";
        static constexpr const char* FFMPEG_PATH = "ffmpeg.exe";

        static const int SETUP_SCREEN_INDEX = 0;
        static const int SETTINGS_SCREEN_INDEX = 1;
        static const int PROCESSING_SCREEN_INDEX = 2;
        static const int ERROR_SCREEN_INDEX = 3;

        static const int OUTPUT_SETTINGS_LINE_MAX_HEIGHT = 305;
        static const int DOWNLOADING_SETTINGS_LINE_MAX_HEIGHT = 105;
        static const int INTERFACE_SETTINGS_LINE_MAX_HEIGHT = 55;

        static constexpr const char* DOWNLOAD_ICON_FILLED_WHITE = ":/SpotifyDownloader/Icons/Download_Icon_W_Filled.png";
        static constexpr const char* DOWNLOAD_ICON_FILLED_COLOUR = ":/SpotifyDownloader/Icons/Download_Icon_Colour_Filled.png";
        static constexpr const char* DOWNLOAD_ICON_WHITE = ":/SpotifyDownloader/Icons/Download_Icon_W.png";
        static constexpr const char* DOWNLOAD_ICON_COLOUR = ":/SpotifyDownloader/Icons/Download_Icon_Colour.png";
        static constexpr const char* ERROR_ICON_FILLED_WHITE = ":/SpotifyDownloader/Icons/Error_Icon_W_Filled.png";
        static constexpr const char* ERROR_ICON_FILLED_COLOUR = ":/SpotifyDownloader/Icons/Error_Icon_Colour_Filled.png";
        static constexpr const char* ERROR_ICON_WHITE = ":/SpotifyDownloader/Icons/Error_Icon_W.png";
        static constexpr const char* ERROR_ICON_COLOUR = ":/SpotifyDownloader/Icons/Error_Icon_Colour.png";
        static constexpr const char* SETTINGS_ICON_FILLED_WHITE = ":/SpotifyDownloader/Icons/SettingsCog_W_Filled.png";
        static constexpr const char* SETTINGS_ICON_FILLED_COLOUR = ":/SpotifyDownloader/Icons/SettingsCog_Colour_Filled.png";
        static constexpr const char* SETTINGS_ICON_WHITE = ":/SpotifyDownloader/Icons/SettingsCog_W.png";
        static constexpr const char* SETTINGS_ICON_COLOUR = ":/SpotifyDownloader/Icons/SettingsCog_Colour.png";

        static constexpr const char* DONATE_ICON_WHITE = ":/SpotifyDownloader/Icons/Donate_Icon_W.png";
        static constexpr const char* DONATE_ICON_COLOUR = ":/SpotifyDownloader/Icons/Donate_Icon_Colour.png";
        static constexpr const char* BUG_ICON_WHITE = ":/SpotifyDownloader/Icons/Bug_Icon_W.png";
        static constexpr const char* BUG_ICON_COLOUR = ":/SpotifyDownloader/Icons/Bug_Icon_Colour.png";
        static constexpr const char* HELP_ICON_WHITE = ":/SpotifyDownloader/Icons/Help_Icon_W.png";
        static constexpr const char* HELP_ICON_COLOUR = ":/SpotifyDownloader/Icons/Help_Icon_Colour.png";

        static constexpr const char* NAMING_TAGS[] = {
            "song name",
            "album name",
            "song artist",
            "song artists",
            "album artist",
            "album artists",
            "track number",
            "song time seconds",
            "song time minutes",
            "song time hours"
        };

        // Output
        static inline bool Overwrite;
    
        static inline Codec::Extension Codec;
        static inline const int CodecIndex() { return _codecIndex; };

        static inline bool NormalizeAudio;
        static inline float NormalizeAudioVolume;
    
        static inline QMap<Codec::Extension, int> AudioBitrate;

        static inline QString PlaylistURL;
        static inline QString SaveLocation;
    
        static inline QString SongOutputFormatTag;
        static inline QString SongOutputFormat;
    
        static inline int FolderSortingIndex;
    
        // Downloading
        static inline bool Notifications;
    
        static inline int ThreadCount;
        static inline float DownloadSpeed;
    
        // Interface
        static inline int DownloaderThreadUIIndex;

        static inline bool SidebarIconsColour;
    public:
        static void SetCodecIndex(int index) { _codecIndex = index; Codec = static_cast<Codec::Extension>(CodecIndex()); };

        static void SetBitrate(int bitrate) { AudioBitrate[Codec] = bitrate; }
        static int GetBitrate() { return AudioBitrate[Codec]; }

        static QStringList Q_NAMING_TAGS();
        static std::tuple<QString, NamingError> FormatOutputNameWithTags(std::function<std::tuple<QString, bool>(QString)> tagHandlerFunc); // Output: (Output, Error), Input: (Tag Replacement, Replacement Is Set)

        static QIcon DownloadIconFilled() { return QIcon(SidebarIconsColour ? DOWNLOAD_ICON_FILLED_COLOUR : DOWNLOAD_ICON_FILLED_WHITE); }
        static QIcon DownloadIcon() { return QIcon(SidebarIconsColour ? DOWNLOAD_ICON_COLOUR : DOWNLOAD_ICON_WHITE); }
        static QIcon ErrorIconFilled() { return QIcon(SidebarIconsColour ? ERROR_ICON_FILLED_COLOUR : ERROR_ICON_FILLED_WHITE); }
        static QIcon ErrorIcon() { return QIcon(SidebarIconsColour ? ERROR_ICON_COLOUR : ERROR_ICON_WHITE); }
        static QIcon SettingsIconFilled() { return QIcon(SidebarIconsColour ? SETTINGS_ICON_FILLED_COLOUR : SETTINGS_ICON_FILLED_WHITE); }
        static QIcon SettingsIcon() { return QIcon(SidebarIconsColour ? SETTINGS_ICON_COLOUR : SETTINGS_ICON_WHITE); }
        static QIcon DonateIcon() { return QIcon(SidebarIconsColour ? DONATE_ICON_COLOUR : DONATE_ICON_WHITE); }
        static QIcon BugIcon() { return QIcon(SidebarIconsColour ? BUG_ICON_COLOUR : BUG_ICON_WHITE); }
        static QIcon HelpIcon() { return QIcon(SidebarIconsColour ? HELP_ICON_COLOUR : HELP_ICON_WHITE); }

        static void SaveSettings();
        static void LoadSettings();
    private:
        static inline QStringList Q_NAMING_TAGS_CACHE;

        static inline int _codecIndex;
};

#endif