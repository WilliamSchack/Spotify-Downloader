#ifndef CODEC_H
#define CODEC_H

#include <QString>
#include <QMap>

class Codec {
	private:
		// FIX WAV CALCULATION
		// https://duc.avid.com/showthread.php?t=232362

		// File size in MB

		static float CalculateBitrateFileSize(int bitrate, int seconds) {
			return (((float)bitrate * (float)seconds) / 8) / 1024; // Bitrate * Seconds
		}

		static float CalculateWAVFileSize(int sampleRate, int seconds) {
			return (44.1 * (16 / 8) * 2 * (float)seconds) / 1024; // Sample Rate * (Bits Per Sample / 8) * Channels * Seconds
		}
	public:
		// Planning to add more, just testing some different codecs for now
		enum class Extension {
			MP3,
			M4A,	 // Still need to implement bitrate, downloading and metadata works
			AAC,	 // Currently implementing, not working | Fix metadata not working, convert to aac from m4a at end
			WAV		 // Still implementing, downloading and metadata works, need to make sampling rate configurable
		};

		enum class ExtensionType {
			MPEG,
			MP4,
			WAV
		};

		struct ExtensionData {
			ExtensionType Type;
			QString String;
			float (*CalculateFileSize)(int bitrate, int seconds);
		};

		static inline const QMap<Extension, ExtensionData> Data {
			{ Extension::MP3, { ExtensionType::MPEG, "mp3", CalculateBitrateFileSize } },
			{ Extension::M4A, { ExtensionType::MP4, "m4a", CalculateBitrateFileSize } },
			{ Extension::AAC, { ExtensionType::MP4, "aac", CalculateBitrateFileSize } },
			{ Extension::WAV, { ExtensionType::WAV, "wav", CalculateWAVFileSize } }
		};
};

#endif