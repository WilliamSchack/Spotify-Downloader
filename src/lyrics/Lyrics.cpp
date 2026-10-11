#include "Lyrics.h"

std::string Lyrics::GetString() const
{
    if (Source == ELyricsSource::None)
        return "";

    if (Type == ELyricsType::Unsynced)
        return UnsyncedLyrics;

    // Convert synched lyrics to a simple lrc file
    std::string lyricsString = "";
    
    for(SynchronisedLyric lyric : SyncedLyrics) {
        int minutes = lyric.StartMs / 60000;
        int seconds = (lyric.StartMs % 60000) / 1000;
        int hundredths = (lyric.StartMs % 1000) / 10;

        std::string minutesString = StringUtils::PadInteger(minutes, 2);
        std::string secondsString = StringUtils::PadInteger(seconds, 2);
        std::string hundredthsString = StringUtils::PadInteger(hundredths, 2);

        std::string lyricTimestamped = "[";
        lyricTimestamped += minutesString + ":";
        lyricTimestamped += secondsString + ".";
        lyricTimestamped += hundredthsString + "]";
        lyricTimestamped += lyric.Lyric + "\n";

        lyricsString += lyricTimestamped;
    }

    return lyricsString;
}