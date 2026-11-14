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
        static inline bool RandomDefaultBackground = true;
        static inline std::string DefaultBackgroundUrl = "";

        // Pool of urls to use for the random default background
        static inline const std::vector<std::string> RANDOM_DEFAULT_BACKGROUND_URLS = {
            "https://open.spotify.com/album/5l4pp1ptAaRFESQfrVMPdu",
            "https://open.spotify.com/album/4cQMG9J5WiIDMYaWf5axzy",
            "https://open.spotify.com/album/0eFoFhftAdNeb0vtTJ0iSB",
            "https://open.spotify.com/album/4S3xeRToG1AjsEWm9Wq97X",
            "https://open.spotify.com/album/2RC4eKAa1X4jdP7kg5aw5M",
            "https://open.spotify.com/album/7fa6VpNPvW2DVbtohJ0x44",
            "https://open.spotify.com/album/3wvtNfppZRg8ASpyPZ4Pqo",
            "https://open.spotify.com/album/5mhMp6LTV40KQhzoAqczkQ",
            "https://open.spotify.com/album/4mQBjeoRBEUHjLUltlyFYH",
            "https://open.spotify.com/album/3NQGQTuE4kjTwbWmUnP74J",
            "https://open.spotify.com/album/4PiXx6k3dIhVXwP26ScmZd",
            "https://open.spotify.com/album/7B2Z50qSHLXDMpuhTJkHYS",
            "https://open.spotify.com/album/7LxvUBCpwq6PnWF7dlN6oU",
            "https://open.spotify.com/album/3W8JRmRDQPiEH77Aq6EtpH",
            "https://open.spotify.com/album/1URR7mNj4qwRxBlQumvpYs",
            "https://open.spotify.com/album/2WIzvoqPNtXtfLzqUNnbxq",
            "https://open.spotify.com/album/7uTSVs0t0qhynCUG36MjvM",
            "https://open.spotify.com/album/78LGjDUuu5dQZRfbrFQ2Ys",
            "https://open.spotify.com/album/4OH1lTy4hxiJzdhD7DGIOf",
            "https://open.spotify.com/album/7jtMWkpwjDlUJPlcJib23P",
            "https://open.spotify.com/album/5VEgJeGRLD1U9p0FIaiWp8",
            "https://open.spotify.com/album/3DVuly5lY0smEWKSXTxrbP",
            "https://open.spotify.com/album/535BQxr12OahhaKGGGS7iK",
            "https://open.spotify.com/album/6jiQFcvq5i7TAHQG1QVBpu",
            "https://open.spotify.com/album/2kGq2PQyEkgcy4zUlBhQVA",
            "https://open.spotify.com/album/0qfndIDNSZnIlisknJFobo",
            "https://open.spotify.com/album/6vxNZbmF7q7k5NPkndRWWh",
            "https://open.spotify.com/album/3p2d2Uvsp8DwCnlWwa0TUm",
            "https://open.spotify.com/album/5BPW34xVtNwez7rVSjWQB3",
            "https://open.spotify.com/album/6rlcgXyaVjkuu6AfaHMn3a",
            "https://open.spotify.com/album/1cxaH3GcgsFC2dYJxicTpR",
            "https://open.spotify.com/album/2wzfAqKAR8tXWovPNbjqLM",
            "https://open.spotify.com/album/1rS3kndeYmd17NDCKHW1Pj",
            "https://open.spotify.com/album/49iDCWueQtoq6srZylDxFJ",
            "https://open.spotify.com/album/3ODN680Xd8ejR1cLBfWPX2",
            "https://open.spotify.com/album/4muEF5biWb506ZojGMfHb7",
            "https://open.spotify.com/album/0EnC43BBsNlC5AZP2UYtgP",
            "https://open.spotify.com/album/3YQA0MVV7U5puzsxGSOhlU",
            "https://open.spotify.com/album/7C9OlFso6Y67XNk53XPatP",
            "https://open.spotify.com/album/2896XcljcjOVEZVXy0u6CI",
            "https://open.spotify.com/album/2dIGnmEIy1WZIcZCFSj6i8",
            "https://open.spotify.com/album/5lEphbceIgaK1XxWeSrC9E",
            "https://open.spotify.com/album/0jJ7mMkCkTe7p9EJgSRxgi",
            "https://open.spotify.com/album/0bCAjiUamIFqKJsekOYuRw",
            "https://open.spotify.com/album/5jJGGsAztgUEXUHHZ2zrah",
            "https://open.spotify.com/album/6f6tko6NWoH00cyFOl4VYQ"
        };

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