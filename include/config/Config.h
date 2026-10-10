#ifndef CONFIG_H
#define CONFIG_H

#include "EExtension.h"
#include "ELyricsSource.h"
#include "ETrackNumberType.h"

#include <string>

class Config
{
    public:
        // Audio
        static inline EExtension CodecExtension = EExtension::MP3;
        static inline bool Normalise = true;
        static inline double NormaliseDb = -14.0;
        static inline bool ManualBitrate = false;
        static inline int BitrateKbps = 128;

        // Metadata
        static inline bool GetLyrics = true;
        static inline std::string ArtistSeperator = "; ";
        static inline ETrackNumberType TrackNumberType = ETrackNumberType::Playlist;

        // File Management
        static inline bool Overwrite = false;
        static inline char FileNameTagsOpeningChar = '<';
        static inline char FileNameTagsClosingChar = '>';
        static inline std::string FileName = "<Song Name> - <Song Artist>";

        // Lyrics File

        // Playlist File

        // Downloading
        static inline int PerDownloadThreads = 6;
        static inline double DownloadSpeedLimit = 0.0;

        // YouTube

        // Interface
        static inline bool AutoOpenDownloadFolder = true;

        // Updates



        // Has the requested lyrics sources with 0 as the highest priority
        static inline constexpr ELyricsSource LYRICS_SOURCE_PRIORITY[] = {
            ELyricsSource::None
        };
};

#endif