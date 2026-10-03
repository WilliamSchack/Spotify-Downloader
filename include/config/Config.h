#ifndef CONFIG_H
#define CONFIG_H

#include "EExtension.h"
#include "ELyricsSource.h"

#include <string>

class Config
{
    public:
        // Testing vars atm
        static inline EExtension CODEC_EXTENSION = EExtension::MP3;
        static inline bool OVERWRITE = false;
        static inline bool NORMALISE = true;
        static inline bool GET_LYRICS = true;
        static inline float NORMALISE_DB = -14.0;
        static inline bool MANUAL_BITRATE = false;
        static inline int BITRATE = 128;
        static inline std::string ARTISTS_SEPERATOR = "; ";
        static inline int PER_DOWNLOAD_THREADS = 6;

        // Has the requested lyrics sources with 0 as the highest priority
        static inline constexpr ELyricsSource LYRICS_SOURCE_PRIORITY[] = {
            ELyricsSource::None
        };
};

#endif