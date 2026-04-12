#ifndef TRACKDATA_H
#define TRACKDATA_H

#include "EPlatform.h"
#include "ArtistData.h"
#include "AlbumData.h"

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

#include <iostream>

struct TrackData
{
    EPlatform Platform = EPlatform::Unknown;
    std::string Id = "";
    std::string Url = "";
    std::string Isrc = "";
    std::string Name = "";
    std::string Description = "";
    std::string ReleaseDate = "";
    std::string ReleaseYear = "";
    bool Explicit = false;
    unsigned int DurationMilliseconds = 0;
    unsigned int DurationSeconds = 0;
    unsigned int DiscNumber = 0;
    unsigned int TrackNumber = 0;
    unsigned int PlaylistTrackNumber = 0;
    AlbumData Album;
    std::vector<ArtistData> Artists;

    TrackData(EPlatform platform) : Platform(platform), Album(platform) {}
    void Print() const;

    void SetDuration(unsigned int ms)
    {
        DurationMilliseconds = ms;
        DurationSeconds = ms / 1000;
    }
};

inline void to_json(nlohmann::json& json, const TrackData& data)
{
    json = {
        {"platform", (int)data.Platform},
        {"id", data.Id},
        {"url", data.Url},
        {"name", data.Name},
        {"description", data.Description},
        {"release_date", data.ReleaseDate},
        {"release_year", data.ReleaseYear},
        {"explicit", data.Explicit},
        {"duration_ms", data.DurationMilliseconds},
        {"duration_s", data.DurationSeconds},
        {"disc_number", data.DiscNumber},
        {"track_number", data.TrackNumber},
        {"playlist_track_number", data.PlaylistTrackNumber},
        {"album", data.Album},
        {"artists", data.Artists}
    };
}

inline void TrackData::Print() const
{
    nlohmann::json json = *this;
    std::cout << json.dump(4) << std::endl;
}

#endif