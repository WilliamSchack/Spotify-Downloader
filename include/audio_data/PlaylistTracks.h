#ifndef PLAYLISTTRACKS_H
#define PLAYLISTTRACKS_H

#include "Logger.h"
#include "PlaylistData.h"
#include "TrackData.h"

#include <nlohmann/json.hpp>

#include <vector>

struct PlaylistTracks
{
    PlaylistData Data;
    std::vector<TrackData> Tracks;

    PlaylistTracks() : Data(EPlatform::Unknown) {}
    nlohmann::json ToJson() const;
    void Print() const;
};

inline void to_json(nlohmann::json& json, const PlaylistTracks& data)
{
    json = {
        {"data", data.Data},
        {"tracks", data.Tracks}
    };
}

inline nlohmann::json PlaylistTracks::ToJson() const
{
    return *this;
}

inline void PlaylistTracks::Print() const
{
    nlohmann::json json = *this;
    Logger::Info(json.dump(4));
}

#endif