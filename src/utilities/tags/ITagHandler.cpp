#include "ITagHandler.h"

TagHandlerResult ITagHandler::FormatString(const std::string& string, const char& openingChar, const char& closingChar)
{
    TagHandlerResult result;

    if (std::isblank(openingChar) || std::isblank(closingChar)) {
        result.Error = ETagError::InvalidEnclosingChars;
        return result;
    }

    std::string formattedString;
    int currentCharIndex = 0;
    while (currentCharIndex <= string.length()) {
        int nextOpeningIndex = string.find(openingChar, currentCharIndex);
        int nextClosingIndex = string.find(closingChar, currentCharIndex);

        if (nextOpeningIndex == -1 || nextClosingIndex == -1) {
            std::string afterTagString = string.substr(currentCharIndex, string.length() - currentCharIndex);
            formattedString.append(afterTagString);

            break;
        }

        std::string beforeTagString = string.substr(currentCharIndex, nextOpeningIndex - currentCharIndex);
        formattedString.append(beforeTagString);

        int tagLength = nextClosingIndex - nextOpeningIndex - 1;
        std::string tag = string.substr(nextOpeningIndex + 1, tagLength);

        TagReplacerResult tagReplacerResult = TagReplacer(tag);
        if (!tagReplacerResult.TagReplaced) {
            result.Error = ETagError::InvalidTag;
            result.InvalidTag = tag;
            return result;
        }

        formattedString.append(tagReplacerResult.Output);

        currentCharIndex = nextClosingIndex + 1;
    }

    result.FormattedString = formattedString;
    return result;
}