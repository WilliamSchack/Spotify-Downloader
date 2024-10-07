#ifndef CODEC_H
#define CODEC_H

#include <QString>
#include <QMap>

class Codec {
	public:
		// Planning to add more, just testing with one different codec for now
		enum class Extension {
			MP3,
			M4A,	 // Still need to implement bitrate, downloading and metadata works
			AAC,	 // Currently implementing, not working | Fix metadata not working, convert to aac from m4a at end
			WAV		 // Currently implementing, not working | Make sampling rate configurable, fix tags not working sometimes
		};

		enum class ExtensionType {
			MPEG,
			MP4,
			WAV
		};

		struct ExtensionData {
			ExtensionType Type;
			QString String;
		};

		static inline const QMap<Extension, ExtensionData> Data {
			{ Extension::MP3, { ExtensionType::MPEG, "mp3" } },
			{ Extension::M4A, { ExtensionType::MP4, "m4a" } },
			{ Extension::AAC, { ExtensionType::MP4, "aac" } },
			{ Extension::WAV, { ExtensionType::WAV, "wav" } }
		};
};

#endif