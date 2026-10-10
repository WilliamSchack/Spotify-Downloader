#include "TrackTagHandler.h"

TrackTagHandler::TrackTagHandler(const TrackData& track) : _track(track) {}

TagReplacerResult TrackTagHandler::TagReplacer(const std::string& tag)
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
        case 0: // Song Name
			tagReplacement = _track.Name;
			break;
		case 1: // Album Name
			tagReplacement = _track.Album.Name;
			break;
		case 2: // Song Artist
			if (_track.Artists.size() > 0)
                tagReplacement = _track.Artists[0].Name;
			break;
		case 3: // Song Artists
			tagReplacement = MetadataManager::CombineArtistNames(_track.Artists, ", ");
			break;
		case 4: // Album Artist
			if (_track.Album.Artists.size() > 0)
                tagReplacement = _track.Album.Artists[0].Name;
			break;
		case 5: // Album Artists
			tagReplacement = MetadataManager::CombineArtistNames(_track.Album.Artists, ", ");
			break;
		case 6: // Codec
			
			break;
		case 7: // Track Number
			tagReplacement = std::to_string(MetadataManager::GetTrackNumber(_track));
			break;
		case 8: // Playlist Track Number
            tagReplacement = std::to_string(_track.PlaylistTrackNumber);
			break;
		case 9: // Album Track Number
			tagReplacement = std::to_string(_track.TrackNumber);
			break;
		case 10: // Disc Number
			tagReplacement = std::to_string(_track.DiscNumber);
			break;
		case 11: // Song Time Seconds
            tagReplacement = std::to_string(_track.DurationSeconds);
			break;
		case 12: // Song Time Minutes
            tagReplacement = _track.GetDurationMinutes();
			break;
		case 13: // Song Time Hours
			tagReplacement = _track.GetDurationHours();
			break;
		case 14: // Year
            tagReplacement = _track.ReleaseYear;
			break;
		case 15: { // Month
			if (_track.ReleaseDate.empty())
                break;

            std::vector<std::string> dateParts = StringUtils::Split(_track.ReleaseDate, "-");
            if (dateParts.size() > 1)
                tagReplacement = dateParts[1];

			break;
        } case 16: { // Day
            if (_track.ReleaseDate.empty())
                break;

            std::vector<std::string> dateParts = StringUtils::Split(_track.ReleaseDate, "-");
            if (dateParts.size() > 2)
                tagReplacement = dateParts[2];

			break;
        }
    }

    result.Output = tagReplacement;
    result.TagReplaced = true;
    return result;
}