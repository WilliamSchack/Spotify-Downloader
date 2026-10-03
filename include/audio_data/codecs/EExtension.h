#ifndef EEXTENSION_H
#define EEXTENSION_H

#include <vector>
#include <string>

enum class EExtension
{
    M4A,
    AAC,
    MP3,
    OGG,
    WAV,
    FLAC,
    WEBM
};

static inline const std::vector<std::string> EEXTENSION_NAMES = {
    "M4A",
    "AAC",
    "MP3",
    "OGG",
    "WAV",
    "FLAC",
    "WEBM"
};

#endif