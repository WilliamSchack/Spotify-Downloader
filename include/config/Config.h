#ifndef CONFIG_H
#define CONFIG_H

#include "EExtension.h"

class Config
{
    public:
        static inline constexpr EExtension CODEC_EXTENSION = EExtension::MP3;
        static inline constexpr float NORMALISE_DB = -14.0;
};

#endif