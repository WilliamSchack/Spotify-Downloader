#ifndef YOUTUBESEARCHRESULT_H
#define YOUTUBESEARCHRESULT_H

#include "EYoutubeCategory.h"
#include "TrackData.h"
#include "AlbumData.h"

#include <nlohmann/json.hpp>

#include <variant>

#include <iostream>

struct YoutubeSearchResult
{
    std::variant<std::monostate, TrackData, AlbumData> Data;

    EYoutubeCategory Category = EYoutubeCategory::None;
    std::string VideoType = "";
    std::string BrowseId = "";
    std::string Views = "";

    void Print() const;
};

inline void to_json(nlohmann::json& json, const YoutubeSearchResult& data)
{
    json = {
        {"category", (int)data.Category},
        {"video_type", data.VideoType},
        {"browse_id", data.BrowseId},
        {"views", data.Views}
    };

    if (std::holds_alternative<TrackData>(data.Data)) {
        json["data"] = std::get<TrackData>(data.Data);
    }
    
    else if (std::holds_alternative<AlbumData>(data.Data)) {
        json["data"] = std::get<AlbumData>(data.Data);
    }
}

inline void YoutubeSearchResult::Print() const
{
    nlohmann::json json = *this;
    std::cout << json.dump(4) << std::endl;
}

#endif