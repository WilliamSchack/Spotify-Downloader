#ifndef ALBUMTRACKS_H
#define ALBUMTRACKS_H

#include "Logger.h"
#include "AlbumData.h"
#include "TrackData.h"

#include <vector>

struct AlbumTracks
{
    AlbumData Data;
    std::vector<TrackData> Tracks;

    AlbumTracks() : Data(EPlatform::Unknown) {}
    nlohmann::json ToJson() const;
    void Print() const;
};

inline void to_json(nlohmann::json& json, const AlbumTracks& data)
{
    json = {
        {"data", data.Data},
        {"tracks", data.Tracks}
    };
}

inline nlohmann::json AlbumTracks::ToJson() const
{
    return *this;
}

inline void AlbumTracks::Print() const
{
    nlohmann::json json = *this;
    Logger::Info(json.dump(4));
}

#endif