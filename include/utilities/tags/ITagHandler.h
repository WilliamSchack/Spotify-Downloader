#ifndef ITAGHANDLER_H
#define ITAGHANDLER_H

#include "TagHandlerResult.h"
#include "TagReplacerResult.h"

#include <vector>
#include <string>

class ITagHandler
{
    public:
        virtual ~ITagHandler() = default;

        TagHandlerResult FormatString(const std::string& string, const char& openingChar, const char& closingChar);
        virtual std::vector<std::string> GetValidTags() const = 0;
    private:
        virtual TagReplacerResult TagReplacer(const std::string& tag) = 0;
};

#endif