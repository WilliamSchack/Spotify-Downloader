#ifndef LYRICS_H
#define LYRICS_H

#include <iomanip>
#include <sstream>
#include <string>
#include <list>
#include <format>

class Lyrics {
	public:
		class SynchronisedLyric {
			public:
				int StartMs;
				int EndMs;
				std::string Lyric;
			public:
				SynchronisedLyric(int startMs, int endMs, std::string lyric) {
					StartMs = startMs;
					EndMs = endMs;
					Lyric = lyric;
				}
		};

		enum class LyricsType {
			None,
			Unsynced,
			Synced
		};
	public:
		LyricsType Type = LyricsType::None;
		std::string UnsyncedLyrics;
		std::list<SynchronisedLyric> SyncedLyrics;
		std::string SourceMessage = "";
	public:
		std::string GetString();
};

#endif