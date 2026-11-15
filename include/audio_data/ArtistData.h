#ifndef ARTISTDATA_H
#define ARTISTDATA_H

#include "Logger.h"
#include "EPlatform.h"
#include "FileUtils.h"

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

    std::string GetUniqueId() const
    {
        std::string uniqueId = "AR" + std::to_string((int)Platform);

        if (Id.empty())
            return uniqueId + FileUtils::ValidateFileName(Name);
        else
            return uniqueId + Id;
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

inline void ArtistData::Print() const
{
    nlohmann::json json = *this;
    Logger::Info(json.dump(4));
}

#endif