#ifndef PLAYLISTTAGHANDLER_H
#define PLAYLISTTAGHANDLER_H

#include "ITagHandler.h"
#include "PlaylistData.h"
#include "VectorUtils.h"
#include "StringUtils.h"

class PlaylistTagHandler : public ITagHandler
{
    public:
        PlaylistTagHandler(const PlaylistData& playlist);

        std::vector<std::string> GetValidTags() const;
    protected:
        TagReplacerResult TagReplacer(const std::string& tag);
    protected:
        PlaylistData _playlist;
    private:
        // TODO: add platform related tags
        static inline const std::vector<std::string> VALID_TAGS {
            "playlist name",
            "playlist owner"
        };
};

#endif