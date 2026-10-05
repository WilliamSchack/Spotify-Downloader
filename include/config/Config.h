#ifndef CONFIG_H
#define CONFIG_H

#include "EExtension.h"
#include "ELyricsSource.h"

#include <string>

class Config
{
    public:
        // Testing vars atm
        static inline EExtension CodecExtension = EExtension::MP3;
        static inline bool Overwrite = false;
        static inline bool Normalise = true;
        static inline bool GetLyrics = true;
        static inline float NormaliseDb = -14.0;
        static inline bool ManualBitrate = false;
        static inline int BitrateKbps = 128;
        static inline std::string ArtistSeperator = "; ";
        static inline int PerDownloadThreads = 6;

        // Has the requested lyrics sources with 0 as the highest priority
        static inline constexpr ELyricsSource LYRICS_SOURCE_PRIORITY[] = {
            ELyricsSource::None
        };
};

#endif