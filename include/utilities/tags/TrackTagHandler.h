#ifndef TRACKTAGHANDLER_H
#define TRACKTAGHANDLER_H

#include "ITagHandler.h"
#include "VectorUtils.h"
#include "StringUtils.h"
#include "MetadataManager.h"
#include "TrackData.h"
#include "ICodec.h"

#include <vector>

class TrackTagHandler : public ITagHandler
{
    public:
        // TODO: add platform related tags
        static inline const std::vector<std::string> VALID_TAGS {
            "song name",
            "album name",
            "song artist",
            "song artists",
            "album artist",
            "album artists",
            "codec",
            "track number",
            "playlist track number",
            "album track number",
            "disc number",
            "song time seconds",
            "song time minutes",
            "song time hours",
            "year",
            "month",
            "day"
        };
    public:
        TrackTagHandler(const TrackData& track, const ICodec* codec = nullptr);
    private:
        TagReplacerResult TagReplacer(const std::string& tag);
    private:
        TrackData _track;
        const ICodec* _codec = nullptr;
};

#endif