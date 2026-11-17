#ifndef ESAMPLE_RATE
#define ESAMPLE_RATE

#include <vector>
#include <string>

// Bit depth will stay at 16-bit, anything more does nothing but pad the file size

enum class ELosslessCodecFormat
{
    Unknown,
    B16H44100,
    B16H48000
};

static inline const std::vector<std::string> ELOSSLESSCODECFORMAT_NAMES = {
    "16-bit 44100Hz",
    "16-bit 48000Hz"
};

#endif