#ifndef CONFIG_H
#define CONFIG_H

#include "EExtension.h"
#include "ELyricsSource.h"
#include "EPlaylistFileType.h"
#include "ETrackNumberType.h"
#include "ELosslessCodecFormat.h"
#include "EIPVersion.h"
#include "YouTubeCookieFile.h"
#include "Paths.h"
#include "VectorUtils.h"
#include "JsonUtils.h"

#include <string>
#include <map>

class Config
{
    public:
        static nlohmann::ordered_json ToJson();
        static void FromJson(const nlohmann::json& json);

        static bool Load();
        static bool Load(const std::filesystem::path& path);
        static bool Save();
        static bool Save(const std::filesystem::path& path);

        static bool YouTubeCookiesValid();
    public:
        // Audio
        static inline EExtension CodecExtension = EExtension::MP3;
        static inline bool Normalise = true;
        static inline double NormaliseDb = -14.0;
        static inline bool ManualBitrate = false;
        static inline std::map<EExtension, int> BitrateKbps = {
            {EExtension::M4A,  128},
            {EExtension::AAC,  128},
            {EExtension::MP3,  192},
            {EExtension::OGG,  128},
            {EExtension::WAV,  0},
            {EExtension::FLAC, 0},
            {EExtension::WEBM, 128}
        };
        static inline std::map<EExtension, ELosslessCodecFormat> LosslessFormats = {
            {EExtension::WAV, ELosslessCodecFormat::B16H44100},
            {EExtension::FLAC, ELosslessCodecFormat::B16H44100}
        };

        // Metadata
        static inline bool GetLyrics = true;
        static inline std::string ArtistSeparator = "; ";
        static inline ETrackNumberType TrackNumberType = ETrackNumberType::Playlist;

        // File Management
        static inline bool Overwrite = false;
        static inline std::filesystem::path DefaultSaveLocation = "";
        static inline char FileNameTagsOpeningChar = '<';
        static inline char FileNameTagsClosingChar = '>';
        static inline std::string FileName = "<Song Name> - <Song Artist>";
        static inline char SubFoldersTagsOpeningChar = '<';
        static inline char SubFoldersTagsClosingChar = '>';
        static inline std::string SubFolders = "";

        // Lyrics File
        static inline bool CreateLyricsFile = false;
        static inline char LyricsFileNameTagsOpeningChar = '<';
        static inline char LyricsFileNameTagsClosingChar = '>';
        static inline std::string LyricsFileName = "<Save Location>/<Song Name> - <Song Artist>";

        // Playlist File
        static inline bool CreatePlaylistFile = false;
        static inline EPlaylistFileType PlaylistFileType = EPlaylistFileType::M3U;
        static inline char PlaylistFileNameTagsOpeningChar = '<';
        static inline char PlaylistFileNameTagsClosingChar = '>';
        static inline std::string PlaylistFileName = "<Save Location>/<Playlist Name> - <Playlist Owner>";

        // Downloading
        static inline int PerDownloadThreads = 6;
        static inline double DownloadSpeedLimit = 0.0;
        static inline int DownloadTimeoutMs = 30000;
        static inline EIPVersion IPVersion = EIPVersion::Automatic;

        // YouTube
        // YouTubeCookies = YouTubeCookieFile.Get()
        static inline std::string YouTubePOToken = "";

        // Interface
        static inline bool ShowStatusNotifications = true;
        static inline bool AutoOpenDownloadFolder = true;

        // Updates
        static inline bool AutoCheckForUpdates = true;
        static inline bool AutoCheckForNotices = true;

        // Background
        static inline std::string DefaultBackgroundUrl = "";

        // Has the requested lyrics sources with 0 as the highest priority
        static inline constexpr ELyricsSource LYRICS_SOURCE_PRIORITY[] = {
            ELyricsSource::Unknown
        };
    private:
        static std::filesystem::path GetFilePath();
    private:
        static inline const std::string FILE_NAME = "Config.json";

        static inline std::filesystem::path _filePathCache = "";
};

#endif