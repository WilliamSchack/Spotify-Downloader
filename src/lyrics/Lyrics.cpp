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

        std::ostringstream paddedMinutesStream;
        std::ostringstream paddedSecondsStream;
        std::ostringstream paddedHundredthsStream;
        
        paddedMinutesStream << std::setw(2) << std::setfill('0') << minutes;
        paddedSecondsStream << std::setw(2) << std::setfill('0') << seconds;
        paddedHundredthsStream << std::setw(2) << std::setfill('0') << hundredths;

        std::string lyricTimestamped = "[";
        lyricTimestamped += paddedMinutesStream.str() + ":";
        lyricTimestamped += paddedSecondsStream.str() + ".";
        lyricTimestamped += paddedHundredthsStream.str() + "]";
        lyricTimestamped += lyric.Lyric + "\n";

        lyricsString += lyricTimestamped;
    }

    return lyricsString;
}