#ifndef SAVABLEPLAYLISTTAGHANDLER_H
#define SAVABLEPLAYLISTTAGHANDLER_H

#include "PlaylistTagHandler.h"
#include "FileUtils.h"

#include <filesystem>

class SavablePlaylistTagHandler : public PlaylistTagHandler
{
    public:
        SavablePlaylistTagHandler(const PlaylistData& playlist);

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