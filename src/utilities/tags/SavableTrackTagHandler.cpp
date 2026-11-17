#include "SavableTrackTagHandler.h"

SavableTrackTagHandler::SavableTrackTagHandler(const TrackData& track) : TrackTagHandler(track) {}

std::vector<std::string> SavableTrackTagHandler::GetValidTags() const
{
    std::vector<std::string> validTags = TrackTagHandler::GetValidTags();
    VectorUtils::AddRange(validTags, VALID_TAGS_SAVABLE);

    return validTags;
}

void SavableTrackTagHandler::SetSaveLocation(const std::filesystem::path& saveLocation)
{
    _saveLocation = saveLocation;
}

TagReplacerResult SavableTrackTagHandler::TagReplacer(const std::string& tag)
{
    TagReplacerResult result;

    std::string tagLower = StringUtils::ToLower(tag);
    int indexOfTag = VectorUtils::IndexOf(VALID_TAGS_SAVABLE, tagLower);
    if (indexOfTag < 0)
        return TrackTagHandler::TagReplacer(tag);

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