#ifndef YTDLPERROR_H
#define YTDLPERROR_H

#include <string>

enum class EYtdlpError
{
    None,
    LowQuality,
    NoFile,
    Unavailable,
    DrmProtected,
    Forbidden,
    IpFlagged,
    InvalidPoToken,
    CookiesExpired,
    Unknown
};

struct YtdlpError
{
    std::string Details = "";
    std::string Parsed = "";
    EYtdlpError Error = EYtdlpError::None;
};

#endif