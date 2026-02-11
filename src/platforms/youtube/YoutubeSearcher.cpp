#include "YoutubeSearcher.h"

#include <iostream>

YoutubeSearcher::YoutubeSearcher() : _yt() {}

PlatformSearcherResult YoutubeSearcher::FindTrack(const TrackData& track)
{
    std::string titleSearchTerm  = track.Name;
    std::string artistSearchTerm = track.Artists[0].Name;
    std::string albumSearchTerm  = track.Album.Name;

    // Remove symbols from query, can return random results otherwise
    // Cant do {S} in std regex so match all acsii symbols
    std::regex symbolsRegex(R"([\x21-\x2F\x3A-\x40\x5B-\x60\x7B-\x7E])");
    titleSearchTerm  = std::regex_replace(titleSearchTerm, symbolsRegex, "");
    artistSearchTerm = std::regex_replace(artistSearchTerm, symbolsRegex, "");
    albumSearchTerm  = std::regex_replace(albumSearchTerm, symbolsRegex, "");

    // If removing the symbols removes the string, set it back to the symbols
    if (titleSearchTerm.empty())  titleSearchTerm = track.Name;
    if (artistSearchTerm.empty()) artistSearchTerm = track.Artists[0].Name;
    if (albumSearchTerm.empty())  albumSearchTerm = track.Album.Name;

    // Multiple queries as song songs only match with quotes, etc.
    std::string searchQueries[] {
        artistSearchTerm + " - " + titleSearchTerm + " - " + albumSearchTerm,
        artistSearchTerm + " - \"" + titleSearchTerm + "\" - " + albumSearchTerm,
        titleSearchTerm + " - " + artistSearchTerm
    };

    // Search for the song
    std::vector<YoutubeSearchResult> searchResults;
    for (const std::string& query : searchQueries) {
        ArrayUtils::ExtendVector(searchResults, _yt.Search(query, EYoutubeCategory::Songs, 4));
        ArrayUtils::ExtendVector(searchResults, _yt.Search(query, EYoutubeCategory::Videos, 4));

        // Search through first album result
        std::vector<YoutubeSearchResult> albumResults = _yt.Search(query, EYoutubeCategory::Albums, 1);
        if (albumResults.size() == 0) continue;

        AlbumTracks albumTracks = _yt.GetAlbum(albumResults[0].BrowseId);
        for (TrackData track : albumTracks.Tracks) {
            YoutubeSearchResult youtubeTrackResult;
            youtubeTrackResult.Category = EYoutubeCategory::Songs;
            if (!track.Description.empty()) {
                // Video type is stored in the track description for albums
                youtubeTrackResult.VideoType = track.Description;
                track.Description = "";
            }

            youtubeTrackResult.Data = track;

            searchResults.push_back(youtubeTrackResult);
        }
    }

    // Score songs


    return PlatformSearcherResult();
}

double YoutubeSearcher::ScoreTrack(const TrackData& src, const TrackData& track)
{

}