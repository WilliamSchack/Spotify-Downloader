#ifndef SEARCHERRESULT_H
#define SEARCHERRESULT_H

#include "TrackData.h"

struct PlatformSearcherResult
{
    TrackData Data;
    double Confidence = 0.0;

    PlatformSearcherResult() : Data(EPlatform::Unknown) {}
    void Print() const;
};

inline void to_json(nlohmann::json& json, const PlatformSearcherResult& data)
{
    json = {
        {"confidence", data.Confidence},
        {"data", data.Data}
    };
}

inline void PlatformSearcherResult::Print() const
{
    nlohmann::json json = *this;
    std::cout << json.dump(4) << std::endl;
}

#endif