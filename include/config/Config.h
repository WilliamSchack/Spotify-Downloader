#ifndef CONFIG_H
#define CONFIG_H

#include "EExtension.h"
#include "ELyricsSource.h"

#include <string>

class Config
{
    public:
        // Testing vars atm
        static inline constexpr EExtension CODEC_EXTENSION = EExtension::M4A;
        static inline constexpr bool NORMALISE = true;
        static inline constexpr bool OVERWRITE = false;
        static inline constexpr float NORMALISE_DB = -14.0;
        static inline constexpr bool MANUAL_BITRATE = false;
        static inline constexpr int BITRATE = 128;
        static inline const std::string ARTISTS_SEPERATOR = "; ";

        // Has the requested lyrics sources with 0 as the highest priority
        static inline constexpr ELyricsSource LYRICS_SOURCE_PRIORITY[] = {

        };
};

#endif