#ifndef PLAYLISTDATA_H
#define PLAYLISTDATA_H

#include "EPlatform.h"
#include "TrackData.h"
#include "ArtistData.h"

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

#include <iostream>

struct PlaylistData
{
    EPlatform Platform = EPlatform::Unknown;
    std::string Id = "";
    std::string Url = "";
    std::string Name = "";
    std::string Description = "";
    unsigned int TotalTracks = 0;
    std::string ImageUrl = "";
    ArtistData Owner;

    PlaylistData(EPlatform platform) : Platform(platform), Owner(platform) {}
    void Print() const;
};

inline void to_json(nlohmann::json& json, const PlaylistData& data)
{
    json = {
        {"platform", (int)data.Platform},
        {"id", data.Id},
        {"url", data.Url},
        {"name", data.Name},
        {"description", data.Description},
        {"total_tracks", data.TotalTracks},
        {"image_url", data.ImageUrl},
        {"owner", data.Owner}
    };
}

inline void PlaylistData::Print() const
{
    nlohmann::json json = *this;
    std::cout << json.dump(4) << std::endl;
}

#endif