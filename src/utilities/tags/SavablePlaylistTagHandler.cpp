#include "SavablePlaylistTagHandler.h"

SavablePlaylistTagHandler::SavablePlaylistTagHandler(const PlaylistData& playlist) : PlaylistTagHandler(playlist) {}

std::vector<std::string> SavablePlaylistTagHandler::GetValidTags() const
{
    std::vector<std::string> validTags = SavablePlaylistTagHandler::GetValidTags();
    VectorUtils::AddRange(validTags, VALID_TAGS_SAVABLE);

    return validTags;
}

void SavablePlaylistTagHandler::SetSaveLocation(const std::filesystem::path& saveLocation)
{
    _saveLocation = saveLocation;
}

TagReplacerResult SavablePlaylistTagHandler::TagReplacer(const std::string& tag)
{
    TagReplacerResult result;

    std::string tagLower = StringUtils::ToLower(tag);
    int indexOfTag = VectorUtils::IndexOf(VALID_TAGS_SAVABLE, tagLower);
    if (indexOfTag < 0)
        return SavablePlaylistTagHandler::TagReplacer(tag);

    std::string tagReplacement = "";
    switch (indexOfTag) {
        case 0: // Save Location
            if (!_saveLocation.empty())
                tagReplacement = FileUtils::PathToUtf8(_saveLocation);
            break;
    }

    result.Output = tagReplacement;
    result.TagReplaced = true;
    return result;
}