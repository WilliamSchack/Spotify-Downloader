#ifndef PLAYLISTDATA_H
#define PLAYLISTDATA_H

#include "Logger.h"
#include "EPlatform.h"
#include "TrackData.h"
#include "ArtistData.h"
#include "FileUtils.h"

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

    std::string GetUniqueId() const
    {
        std::string uniqueId = "P" + std::to_string((int)Platform);

        if (Id.empty()) {
            uniqueId += FileUtils::ValidateFileName(Name);
            if (Owner.Platform != EPlatform::Unknown)
                uniqueId += Owner.GetUniqueId();

            return uniqueId;
        } else
            return uniqueId + Id;
    }
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
    Logger::Info(json.dump(4));
}

#endif