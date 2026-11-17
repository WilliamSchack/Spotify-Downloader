#ifndef SEARCHERRESULT_H
#define SEARCHERRESULT_H

#include "Logger.h"
#include "TrackData.h"

struct PlatformSearcherResult
{
    TrackData Data;
    double Confidence = 0.0;

    PlatformSearcherResult() : Data(EPlatform::Unknown) {}
    nlohmann::json ToJson() const;
    void Print() const;
};

inline void to_json(nlohmann::json& json, const PlatformSearcherResult& data)
{
    json = {
        {"confidence", data.Confidence},
        {"data", data.Data}
    };
}

inline nlohmann::json PlatformSearcherResult::ToJson() const
{
    return *this;
}

inline void PlatformSearcherResult::Print() const
{
    nlohmann::json json = *this;
    Logger::Info(json.dump(4));
}

#endif