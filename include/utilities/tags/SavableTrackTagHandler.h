#ifndef ABSOLUTETRACKTAGHANDLER_H
#define ABSOLUTETRACKTAGHANDLER_H

#include "TrackTagHandler.h"
#include "FileUtils.h"

#include <filesystem>

class SavableTrackTagHandler : public TrackTagHandler
{
    public:
        SavableTrackTagHandler(const TrackData& track);

        std::vector<std::string> GetValidTags() const override;
        void SetSaveLocation(const std::filesystem::path& saveLocation);
    private:
        TagReplacerResult TagReplacer(const std::string& tag) override;
    private:
        static inline const std::vector<std::string> VALID_TAGS_SAVABLE {
            "save location"
        };

        std::filesystem::path _saveLocation;
};

#endif