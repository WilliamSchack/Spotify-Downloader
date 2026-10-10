#ifndef ITAGHANDLER_H
#define ITAGHANDLER_H

#include "TagHandlerResult.h"
#include "TagReplacerResult.h"

class ITagHandler
{
    public:
        virtual ~ITagHandler() = default;

        TagHandlerResult FormatString(const std::string& string, const char& openingChar, const char& closingChar);
        virtual TagReplacerResult TagReplacer(std::string tag);
};

#endif