#ifndef YTDLPERROR_H
#define YTDLPERROR_H

#include "EYtdlpError"

#include <string>

struct YtdlpError
{
    std::string Details = "";
    std::string Parsed = "";
    EYtdlpError Error = EYtdlpError::None;
};

#endif