#include "PlaylistTagHandler.h"

PlaylistTagHandler::PlaylistTagHandler(const PlaylistData& playlist) : _playlist(playlist) {}

std::vector<std::string> PlaylistTagHandler::GetValidTags() const
{
    return VALID_TAGS;
}

TagReplacerResult PlaylistTagHandler::TagReplacer(const std::string& tag)
{
    TagReplacerResult result;
    
    std::string tagLower = StringUtils::ToLower(tag);
    int indexOfTag = VectorUtils::IndexOf(VALID_TAGS, tagLower);
    if (indexOfTag < 0) {
        result.TagReplaced = false;
        return result;
    }

    // Def a better way to do this, but this works
    // TODO: Maybe find out a way to make the separators customisable?
    std::string tagReplacement = "";
    switch (indexOfTag) {
        case 0: // Playlist Name
			tagReplacement = _playlist.Name;
			break;
		case 1: // Playlist Owner
			tagReplacement = _playlist.Owner.Name;
			break;
    }

    result.Output = tagReplacement;
    result.TagReplaced = true;
    return result;
}