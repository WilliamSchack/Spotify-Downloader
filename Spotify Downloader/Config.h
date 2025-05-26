#ifndef CONFIG_H
#define CONFIG_H

#define QT_MESSAGELOGCONTEXT
#include "Utilities/Logger.h"

#include "Downloading/Codec.h"

#include "Utilities/StringUtils.h"

#include "Network/SpotifyAPI.h"

#include <QSettings>
#include <QJsonObject>
#include <QMap>
#include <QIcon>
#include <QMovie>

class Config {
    public:
        enum class NamingError {
            None,
            EnclosingTagsInvalid,
            TagInvalid
        };

        static inline const QString ORGANIZATION_NAME = "WilliamSchack";
        static inline const QString APPLICATION_NAME = "Spotify Downloader";

        static inline const QString YTDLP_PATH = "yt-dlp.exe";
        static inline const QString FFMPEG_PATH = "ffmpeg.exe";

        static const int SETUP_SCREEN_INDEX = 0;
        static const int SETTINGS_SCREEN_INDEX = 1;
        static const int PROCESSING_SCREEN_INDEX = 2;
        static const int ERROR_SCREEN_INDEX = 3;

        static inline const QString DOWNLOAD_ICON_FILLED_WHITE = ":/SpotifyDownloader/Icons/Download_Icon_W_Filled.png";
        static inline const QString DOWNLOAD_ICON_FILLED_COLOUR = ":/SpotifyDownloader/Icons/Download_Icon_Colour_Filled.png";
        static inline const QString DOWNLOAD_ICON_WHITE = ":/SpotifyDownloader/Icons/Download_Icon_W.png";
        static inline const QString DOWNLOAD_ICON_COLOUR = ":/SpotifyDownloader/Icons/Download_Icon_Colour.png";
        static inline const QString ERROR_ICON_FILLED_WHITE = ":/SpotifyDownloader/Icons/Error_Icon_W_Filled.png";
        static inline const QString ERROR_ICON_FILLED_COLOUR = ":/SpotifyDownloader/Icons/Error_Icon_Colour_Filled.png";
        static inline const QString ERROR_ICON_WHITE = ":/SpotifyDownloader/Icons/Error_Icon_W.png";
        static inline const QString ERROR_ICON_COLOUR = ":/SpotifyDownloader/Icons/Error_Icon_Colour.png";
        static inline const QString SETTINGS_ICON_FILLED_WHITE = ":/SpotifyDownloader/Icons/SettingsCog_W_Filled.png";
        static inline const QString SETTINGS_ICON_FILLED_COLOUR = ":/SpotifyDownloader/Icons/SettingsCog_Colour_Filled.png";
        static inline const QString SETTINGS_ICON_WHITE = ":/SpotifyDownloader/Icons/SettingsCog_W.png";
        static inline const QString SETTINGS_ICON_COLOUR = ":/SpotifyDownloader/Icons/SettingsCog_Colour.png";

        static inline const QString UPDATE_ICON_WHITE = ":/SpotifyDownloader/Icons/Update_W.png";
        static inline const QString UPDATE_ICON_COLOUR = ":/SpotifyDownloader/Icons/Update_Colour.png";
        static inline const QString UPDATE_ROTATING_ICON_WHITE = ":/SpotifyDownloader/Icons/UpdateRotating_W.gif";
        static inline const QString UPDATE_ROTATING_ICON_COLOUR = ":/SpotifyDownloader/Icons/UpdateRotating_Colour.gif";
        static inline const QString UPDATE_AVAILABLE_ICON_WHITE = ":/SpotifyDownloader/Icons/UpdateAvailable_W.png";
        static inline const QString UPDATE_AVAILABLE_ICON_COLOUR = ":/SpotifyDownloader/Icons/UpdateAvailable_Colour.png";
        static inline const QString UPDATE_UP_TO_DATE_ICON_WHITE = ":/SpotifyDownloader/Icons/UpdateUpToDate_W.png";
        static inline const QString UPDATE_UP_TO_DATE_ICON_COLOUR = ":/SpotifyDownloader/Icons/UpdateUpToDate_Colour.png";

        static inline const QString DONATE_ICON_WHITE = ":/SpotifyDownloader/Icons/Donate_Icon_W.png";
        static inline const QString DONATE_ICON_COLOUR = ":/SpotifyDownloader/Icons/Donate_Icon_Colour.png";
        static inline const QString BUG_ICON_WHITE = ":/SpotifyDownloader/Icons/Bug_Icon_W.png";
        static inline const QString BUG_ICON_COLOUR = ":/SpotifyDownloader/Icons/Bug_Icon_Colour.png";
        static inline const QString HELP_ICON_WHITE = ":/SpotifyDownloader/Icons/Help_Icon_W.png";
        static inline const QString HELP_ICON_COLOUR = ":/SpotifyDownloader/Icons/Help_Icon_Colour.png";

        static inline const QStringList NAMING_TAGS {
            "song name",
            "album name",
            "song artist",
            "song artists",
            "album artist",
            "album artists",
            "codec",
            "track number",
            "playlist track number",
            "album track number",
            "disk number",
            "song time seconds",
            "song time minutes",
            "song time hours",
            "year",
            "month",
            "day"
        };

        // Output
        static inline QString PlaylistURL;
        static inline QString SaveLocation;

        static inline bool Overwrite;
    
        static inline Codec::Extension Codec;
        static inline const int CodecIndex() { return _codecIndex; };

        static inline int TrackNumberIndex;

        static inline bool NormalizeAudio;
        static inline float NormalizeAudioVolume;
    
        static inline QMap<Codec::Extension, int> AudioBitrate;
    
        static inline QString ArtistSeparator;

        static inline QString FileNameTag;
        static inline QString FileName;
    
        static inline QString SubFoldersTag;
        static inline QString SubFolders;
    
        // Downloading
        static inline bool Notifications;
    
        static inline int ThreadCount;
        static inline float DownloadSpeed;

        static inline QString YouTubeCookies;
        static inline QString POToken;
        
        // Only used when cookies are assigned, premium is assumed
        // If premium is not detected during a download, this will be set to false
        // Getting this value before the first download will be inaccurate.
        static inline bool HasPremium = true;
    
        // Interface
        static inline int DownloaderThreadUIIndex;

        static inline bool AutoOpenDownloadFolder;

        static inline bool SidebarIconsColour;

        static inline bool CheckForUpdates;
    public:
        static void SetCodecIndex(int index) { _codecIndex = index; Codec = static_cast<Codec::Extension>(CodecIndex()); };

        static void SetBitrate(int bitrate) { AudioBitrate[Codec] = bitrate; }
        static int GetBitrate() { return AudioBitrate[Codec]; }

        static std::tuple<QString, NamingError> FormatStringWithTags(QString stringTag, QString string, std::function<std::tuple<QString, bool>(QString)> tagHandlerFunc); // Output: (Output, Error), Input: (Tag Replacement, Replacement Is Set)

        static QIcon DownloadIconFilled() { return QIcon(SidebarIconsColour ? DOWNLOAD_ICON_FILLED_COLOUR : DOWNLOAD_ICON_FILLED_WHITE); }
        static QIcon DownloadIcon() { return QIcon(SidebarIconsColour ? DOWNLOAD_ICON_COLOUR : DOWNLOAD_ICON_WHITE); }
        static QIcon ErrorIconFilled() { return QIcon(SidebarIconsColour ? ERROR_ICON_FILLED_COLOUR : ERROR_ICON_FILLED_WHITE); }
        static QIcon ErrorIcon() { return QIcon(SidebarIconsColour ? ERROR_ICON_COLOUR : ERROR_ICON_WHITE); }
        static QIcon SettingsIconFilled() { return QIcon(SidebarIconsColour ? SETTINGS_ICON_FILLED_COLOUR : SETTINGS_ICON_FILLED_WHITE); }
        static QIcon SettingsIcon() { return QIcon(SidebarIconsColour ? SETTINGS_ICON_COLOUR : SETTINGS_ICON_WHITE); }

        static QPixmap UpdateIcon() { return QPixmap(SidebarIconsColour ? UPDATE_ICON_COLOUR : UPDATE_ICON_WHITE); }
        static QMovie* UpdateRotatingIcon() { return new QMovie(SidebarIconsColour ? UPDATE_ROTATING_ICON_COLOUR : UPDATE_ROTATING_ICON_WHITE); }
        static QPixmap UpdateAvailableIcon() { return QPixmap(SidebarIconsColour ? UPDATE_AVAILABLE_ICON_COLOUR : UPDATE_AVAILABLE_ICON_WHITE); }
        static QPixmap UpdateUpToDateIcon() { return QPixmap(SidebarIconsColour ? UPDATE_UP_TO_DATE_ICON_COLOUR : UPDATE_UP_TO_DATE_ICON_WHITE); }

        static QIcon DonateIcon() { return QIcon(SidebarIconsColour ? DONATE_ICON_COLOUR : DONATE_ICON_WHITE); }
        static QIcon BugIcon() { return QIcon(SidebarIconsColour ? BUG_ICON_COLOUR : BUG_ICON_WHITE); }
        static QIcon HelpIcon() { return QIcon(SidebarIconsColour ? HELP_ICON_COLOUR : HELP_ICON_WHITE); }

        static void SaveSettings();
        static void LoadSettings();
    private:
        static QJsonObject SettingsLog();

        static inline QStringList Q_NAMING_TAGS_CACHE;

        static inline int _codecIndex;
};

#endif