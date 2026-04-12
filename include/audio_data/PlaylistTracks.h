#ifndef PLAYLISTTRACKS_H
#define PLAYLISTTRACKS_H

#include "PlaylistData.h"
#include "TrackData.h"

#include <nlohmann/json.hpp>

#include <vector>

struct PlaylistTracks
{
    PlaylistData Data;
    std::vector<TrackData> Tracks;

    PlaylistTracks() : Data(EPlatform::Unknown) {}
    void Print() const;
};

inline void to_json(nlohmann::json& json, const PlaylistTracks& data)
{
    json = {
        {"data", data.Data},
        {"tracks", data.Tracks}
    };
}

inline void PlaylistTracks::Print() const
{
    nlohmann::json json = *this;
    std::cout << json.dump(4) << std::endl;
}

#endif