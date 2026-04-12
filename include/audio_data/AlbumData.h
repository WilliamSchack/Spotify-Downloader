#ifndef ALBUMDATA_H
#define ALBUMDATA_H

#include "EPlatform.h"
#include "EAlbumType.h"
#include "ArtistData.h"

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

#include <iostream>

struct AlbumData
{
    EPlatform Platform = EPlatform::Unknown;
    std::string Id = "";
    std::string Url = "";
    std::string Name = "";
    std::string Description = "";
    unsigned int TotalTracks = 0;
    std::string ImageUrl = "";
    std::string ReleaseDate = "";
    std::string ReleaseYear = "";
    EAlbumType Type = EAlbumType::Album;
    std::vector<ArtistData> Artists;

    AlbumData(EPlatform platform) : Platform(platform) {}
    void Print() const;

    void SetMainArtist(ArtistData artist)
    {
        Artists = std::vector<ArtistData>();
        Artists.push_back(artist);
    }
};

inline void to_json(nlohmann::json& json, const AlbumData& data)
{
    json = {
        {"platform", (int)data.Platform},
        {"id", data.Id},
        {"url", data.Url},
        {"name", data.Name},
        {"description", data.Description},
        {"total_tracks", data.TotalTracks},
        {"image_url", data.ImageUrl},
        {"release_date", data.ReleaseDate},
        {"release_year", data.ReleaseYear},
        {"type", (int)data.Type},
        {"artists", data.Artists}
    };
}

inline void AlbumData::Print() const
{
    nlohmann::json json = *this;
    std::cout << json.dump(4) << std::endl;
}

#endif