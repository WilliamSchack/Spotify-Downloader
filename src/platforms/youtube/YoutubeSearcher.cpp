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

    // Get the best result
    return GetClosestTrack(track, searchResults);
}

// TODO: Normalise the score
float YoutubeSearcher::ScoreTrack(const TrackData& src, const TrackData& track)
{
    if (track.Id.empty()) return 0;

    // Must be inside the allowed time range
    if (track.DurationSeconds == 0 ||
        track.DurationSeconds < src.DurationSeconds - SECONDS_DIFFERENCE_ALLOWED ||
        track.DurationSeconds > src.DurationSeconds + SECONDS_DIFFERENCE_ALLOWED){
        return -1;
    }

    // Score track
    float totalScore = 0;

    // Check the similarities of the names
    std::string nameLower = src.Name;
    std::string secondNameLower = track.Name;
    StringUtils::ToLower(nameLower);
    StringUtils::ToLower(secondNameLower);

    float titleRegularScore = StringUtils::LevenshteinDistanceSimilarity(src.Name, track.Name);
    float titleLowerScore = StringUtils::LevenshteinDistanceSimilarity(nameLower, secondNameLower);
    float titleScore = std::max(titleRegularScore, titleLowerScore); // Use the higher score
    totalScore += titleScore;

    // Time score
    double timeScore = MathUtils::Lerp(0, 1, (SECONDS_DIFFERENCE_ALLOWED - std::abs((int)track.DurationSeconds - (int)src.DurationSeconds)) / SECONDS_DIFFERENCE_ALLOWED);
    totalScore += timeScore;

    // If the title and time scores are low, no point continuing
    if (titleScore < 0.5 && timeScore < 0.75)
        return 0;

    // Score artists individually and add similarities to the total score
    if (track.Artists.size() == 0)
        return 0;

    // Use the most amount of artists to divide, this decreases the score if the artist count is not the same 
    int highestArtistCount = std::max(src.Artists.size(), track.Artists.size());

    float totalArtistScoreIncrease = 1.5;
    float currentArtistsScoreTotal = 0;

    std::vector<ArtistData> artistsToScore = track.Artists;
    for (ArtistData artist : src.Artists) {
        if (artistsToScore.size() == 0)
            break;

        // Check each found artist left to score and add the highest similarity to the artist score total
        float highestArtistScore = 0;
        int highestArtistIndex = 0;
        for (int i = 0; i < artistsToScore.size(); i++) {
            std::string artistToScoreName = artistsToScore[i].Name;
            float artistScore = StringUtils::LevenshteinDistanceSimilarity(artist.Name, artistToScoreName);

            if (artistScore > highestArtistScore) {
                highestArtistScore = artistScore;
                highestArtistIndex = i;
            }
        }

        ArrayUtils::RemoveAt(artistsToScore, highestArtistIndex);
        currentArtistsScoreTotal += highestArtistScore;
    }

    // Divide the scores based on the amount of found artists, and max it out at the total available score
    currentArtistsScoreTotal /= highestArtistCount;
    currentArtistsScoreTotal *= totalArtistScoreIncrease;

    totalScore += currentArtistsScoreTotal;

    // Album score
    if (track.Album.Name == "") {
        // If no target album name, compare the source album name to the name with less of an impact
        float albumScore = StringUtils::LevenshteinDistanceSimilarity(track.Name, src.Album.Name);
        totalScore += albumScore * 0.3; 
    } else {
        // Check the similarity of album names and add to score
        float albumScore = StringUtils::LevenshteinDistanceSimilarity(track.Album.Name, src.Album.Name);
        totalScore += albumScore * 0.6;
    }

    // Title score
    if (track.Name != "") {
        bool banned = false;

        for (std::string keyword : BANNED_TITLE_KEYWORDS) {
            for (std::string format : BANNED_TITLE_KEYWORDS_FORMATS) {
                std::string word = StringUtils::ReplaceAll(format, "%", keyword);
                if (StringUtils::Contains(nameLower, word) != StringUtils::Contains(secondNameLower, word)) {
                    banned = true;
                    break;
                }
            }

            if (banned) break;
        }

        if (banned)
            return 0;

        // Add score an artist name is in the title
        for (ArtistData artist : src.Artists) {
            if (StringUtils::Contains(src.Name, artist.Name)) {
                totalScore += 0.2;
                break;
            }
        }
    }

    return totalScore;
}

PlatformSearcherResult YoutubeSearcher::GetClosestTrack(const TrackData& target, std::vector<YoutubeSearchResult> tracks)
{
    std::vector<PlatformSearcherResult> finalResults;

    std::vector<std::string> bannedIDs;
    std::vector<std::string> scoredIDs;

    for (YoutubeSearchResult result : tracks) {
        TrackData currentTrack = std::get<TrackData>(result.Data);

        // Only allow explicit songs if the track is explicit (YTMusicAPI does not return this for videos)
        if (result.Category != EYoutubeCategory::Videos && target.Explicit != currentTrack.Explicit)
            continue;

        // Must have a valid id
        if (ArrayUtils::Contains(bannedIDs, currentTrack.Id)) continue;
        if (ArrayUtils::Contains(scoredIDs, currentTrack.Id)) continue;

        // Score
        double score = ScoreTrack(target, currentTrack);
        if (score == 0) continue;
        if (score < 0) {
            bannedIDs.push_back(currentTrack.Id);
            continue;
        }

        scoredIDs.push_back(currentTrack.Id);

        PlatformSearcherResult finalResult;
        finalResult.Data = currentTrack;
        finalResult.Confidence = score;

        finalResults.push_back(finalResult);
    }

    // Double check banned IDs, songs may have scored with a lower time before it was added
    for (int i = finalResults.size() - 1; i >= 0; i--) {
        std::string resultId = finalResults[i].Data.Id;
        if (ArrayUtils::Contains(bannedIDs, resultId))
            ArrayUtils::RemoveAt(finalResults, i);
    }

    // Get the result with the highest score
    PlatformSearcherResult finalResult;
    for (const PlatformSearcherResult& result : finalResults) {
        if (result.Confidence > finalResult.Confidence)
            finalResult = result;
    }

    return finalResult;
}