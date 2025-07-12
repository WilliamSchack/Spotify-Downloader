#include "Lyrics.h"

std::string Lyrics::GetString() {
	if (Type == LyricsType::None)
		return "";

	switch (Type) {
		case LyricsType::Unsynced :
			return UnsyncedLyrics;
		case LyricsType::Synced:
			// Translate synced lyrics to simple LRC
			std::string lyricsString = "";
			for(SynchronisedLyric lyric : SyncedLyrics) {
				int minutes = lyric.StartMs / 60000;
				int seconds = (lyric.StartMs % 60000) / 1000;
				int hundredths = (lyric.StartMs % 1000) / 10;

				std::ostringstream paddedMinutesStream;
				std::ostringstream paddedSecondsStream;
				std::ostringstream paddedHuntredthsStream;
				
				paddedMinutesStream << std::setw(2) << std::setfill('0') << minutes;
				paddedSecondsStream << std::setw(2) << std::setfill('0') << seconds;
				paddedHuntredthsStream << std::setw(2) << std::setfill('0') << hundredths;

				std::string lyricTimestamped = std::format(
					"[{}:{}.{}]{}\n",
					paddedMinutesStream.str(),
					paddedSecondsStream.str(),
					paddedHuntredthsStream.str(),
					lyric.Lyric
				);

				lyricsString += lyricTimestamped;
			}
			return lyricsString;
	}

	return "";
}