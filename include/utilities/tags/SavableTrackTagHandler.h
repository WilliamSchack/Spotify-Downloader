#ifndef ABSOLUTETRACKTAGHANDLER_H
#define ABSOLUTETRACKTAGHANDLER_H

#include "TrackTagHandler.h"
#include "VectorUtils.h"
#include "FileUtils.h"

#include <filesystem>

class SavableTrackTagHandler : public TrackTagHandler
{
    public:
        SavableTrackTagHandler(const TrackData& track, const std::filesystem::path& saveLocation);
        std::vector<std::string> GetValidTags() const override;
    private:
        TagReplacerResult TagReplacer(const std::string& tag) override;
    private:
        static inline const std::vector<std::string> VALID_TAGS_ABSOLUTE {
            "save location"
        };

        std::filesystem::path _saveLocation;
};

#endif