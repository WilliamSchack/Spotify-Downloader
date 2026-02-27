#ifndef YOUTUBESEARCHER_H
#define YOUTUBESEARCHER_H

#include "IPlatformSearcher.h"
#include "YTMusicAPI.h"
#include "ArrayUtils.h"
#include "MathUtils.h"

#include <regex>

class YoutubeSearcher : public IPlatformSearcher
{
    public:
        YoutubeSearcher();

        PlatformSearcherResult FindTrack(const TrackData& track) override;
        AlbumTracks FindAlbum(const AlbumTracks& track) override { return AlbumTracks(); };
        PlaylistTracks FindPlaylist(const PlaylistTracks& track) override { return PlaylistTracks(); };
    private:
        float ScoreTrack(const TrackData& src, const TrackData& track);
        PlatformSearcherResult GetClosestTrack(const TrackData& target, std::vector<YoutubeSearchResult> tracks);
    private:
        static inline constexpr float SECONDS_DIFFERENCE_ALLOWED = 15.0;

        // Keywords that are not allowed in a song title if they dont exist in the source
        static inline const std::string BANNED_TITLE_KEYWORDS[] = { "reverse", "instrumental" };
        static inline const std::string BANNED_TITLE_KEYWORDS_FORMATS[] = { " % ", "(%", "%)", "%" };

        YTMusicAPI _yt;
};

#endif