#ifndef TAGHANDLERRESULT_H
#define TAGHANDLERRESULT_H

#include <string>

enum class ETagError
{
    None,
    InvalidEnclosingChars,
    InvalidTag
};

struct TagHandlerResult
{
    std::string FormattedString;

    ETagError Error;
    std::string InvalidTag;
};

#endif