#ifndef YTDLPERROR_H
#define YTDLPERROR_H

#include <string>

enum class EYtdlpError
{
    None,
    Unknown,
    LowQuality,
    NoFile,
    Unavailable,
    DrmProtected,
    Forbidden,
    IpFlagged,
    InvalidPoToken,
    CookiesExpired
};

struct YtdlpError
{
    std::string Details = "";
    std::string Parsed = "";
    EYtdlpError Error = EYtdlpError::None;
};

#endif