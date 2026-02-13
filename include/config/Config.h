#ifndef CONFIG_H
#define CONFIG_H

#include "EExtension.h"

class Config
{
    public:
        // Testing vars atm
        static inline constexpr EExtension CODEC_EXTENSION = EExtension::MP3;
        static inline constexpr float NORMALISE_DB = -14.0;
        static inline constexpr bool MANUAL_BITRATE = false;
        static inline constexpr int BITRATE = 128;
};

#endif