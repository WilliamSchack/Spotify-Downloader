#ifndef ALBUMTRACKS_H
#define ALBUMTRACKS_H

#include "AlbumData.h"
#include "TrackData.h"

#include <vector>

struct AlbumTracks
{
    AlbumData Data;
    std::vector<TrackData> Tracks;

    AlbumTracks() : Data(EPlatform::Unknown) {}
    void Print() const;
};

inline void to_json(nlohmann::json& json, const AlbumTracks& data)
{
    json = {
        {"data", data.Data},
        {"tracks", data.Tracks}
    };
}

inline void AlbumTracks::Print() const
{
    nlohmann::json json = *this;
    std::cout << json.dump(4) << std::endl;
}

#endif