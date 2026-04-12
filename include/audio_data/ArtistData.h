#ifndef ARTISTDATA_H
#define ARTISTDATA_H

#include "EPlatform.h"

#include <nlohmann/json.hpp>

#include <string>

#include <iostream>

struct ArtistData
{
    EPlatform Platform = EPlatform::Unknown;
    std::string Id = "";
    std::string Url = "";
    std::string Name = "";

    ArtistData(EPlatform platform) : Platform(platform) {}
    void Print() const;
};

inline void to_json(nlohmann::json& json, const ArtistData& data)
{
    json = {
        {"platform", (int)data.Platform},
        {"id", data.Id},
        {"url", data.Url},
        {"name", data.Name}
    };
}

inline void ArtistData::Print() const
{
    nlohmann::json json = *this;
    std::cout << json.dump(4) << std::endl;
}

#endif