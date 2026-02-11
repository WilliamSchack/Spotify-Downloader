#ifndef YOUTUBESEARCHRESULT_H
#define YOUTUBESEARCHRESULT_H

#include "EYoutubeCategory.h"
#include "TrackData.h"
#include "AlbumData.h"

#include <variant>

class YoutubeSearchResult
{
    std::variant<TrackData, AlbumData> Data;

    EYoutubeCategory Category = EYoutubeCategory::None;
    std::string BrowseId = "";
    std::string PlaylistId = "";
    std::string Views = "";
};

#endif