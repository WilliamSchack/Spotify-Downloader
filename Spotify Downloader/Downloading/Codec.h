#ifndef CODEC_H
#define CODEC_H

#include <QString>
#include <QMap>

class Codec {
	public:
		// Planning to add more, just testing with one different codec for now
		enum Extension {
			MP3,
			M4A, // Currently implementing, not working
			AAC	 // Currently implementing, not working
		};

		static inline const QMap<Extension, QString> Strings{
			{ Extension::MP3, "mp3" },
			{ Extension::M4A, "m4a" },
			{ Extension::AAC, "aac" }
		};
};

#endif