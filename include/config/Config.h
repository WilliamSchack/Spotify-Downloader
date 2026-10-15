#ifndef CONFIG_H
#define CONFIG_H

#include "EExtension.h"
#include "ELyricsSource.h"
#include "EPlaylistFileType.h"
#include "ETrackNumberType.h"
#include "EIPVersion.h"
#include "YouTubeCookieFile.h"

#include <string>
#include <map>

class Config
{
    public:
        static bool YouTubeCookiesValid();
    public:
        // Audio
        static inline EExtension CodecExtension = EExtension::MP3;
        static inline bool Normalise = true;
        static inline double NormaliseDb = -14.0;
        static inline bool ManualBitrate = false;
        static inline std::map<EExtension, int> BitrateKbps = {
            {EExtension::M4A, 128},
            {EExtension::AAC, 128},
            {EExtension::MP3, 192},
            {EExtension::OGG, 128},
            {EExtension::WAV, 0},  // TODO: Add sample rate settings
            {EExtension::FLAC, 0}, // TODO: Add sample rate settings
            {EExtension::WEBM, 0}  // TODO: IMPLEMENT WEBM
        };

        // Metadata
        static inline bool GetLyrics = true;
        static inline std::string ArtistSeperator = "; ";
        static inline ETrackNumberType TrackNumberType = ETrackNumberType::Playlist;

        // File Management
        static inline bool Overwrite = false;
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
        static inline EPlaylistFileType PlaylistFileType = EPlaylistFileType::None;
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
        static inline bool AutoOpenDownloadFolder = true;

        // Updates



        // Has the requested lyrics sources with 0 as the highest priority
        static inline constexpr ELyricsSource LYRICS_SOURCE_PRIORITY[] = {
            ELyricsSource::Unknown
        };
};

#endif