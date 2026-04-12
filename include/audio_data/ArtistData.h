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

    void Print() const
    {
        std::cout << (int)Platform << std::endl;
        std::cout << Id << std::endl;
        std::cout << Url << std::endl;
        std::cout << Name << std::endl;
    }
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

#endif