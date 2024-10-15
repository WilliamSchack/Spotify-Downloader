#ifndef CODEC_H
#define CODEC_H

#include <QString>
#include <QMap>

class Codec {
	private:
		// File size in MB

		static float CalculateBitrateFileSize(int bitrate, int seconds) {
			return (((float)bitrate * (float)seconds) / 8) / 1024; // Bitrate * Seconds
		}

		static float CalculateWAVFileSize(int bitrate, int seconds) {
			return (44.1 * (16 / 8) * 2 * (float)seconds) / 1024; // Sample Rate * (Bits Per Sample / 8) * Channels * Seconds
		}
	public:
		// Planning to add more, just testing some different codecs for now
		enum class Extension {
			MP3,	 // Done
			M4A,	 // Still need to implement bitrate, downloading and metadata works, cover art is being weird giving errors sometimes
			AAC,	 // Currently implementing, not working | Fix metadata not working, convert to aac from m4a at end
			WAV		 // Essentially done, normalising hung last download, test that
		};

		enum class MetadataType {
			MPEG,
			MP4,
			RIFF	// Not all programs support RIFF chunks
		};

		struct ExtensionData {
			MetadataType Type;	// How the metadata is handled
			QString String;		// String used at end of files

			QString FFMPEGFormat;			// When using FFMPEG for changing file, if different format needed, use this format
			QString FFMPEGConversionParams;	// If codec needs to be converted, use these parameters

			bool LockedBitRate = false; // If bitrate does not need to be set

			float (*CalculateFileSize)(int bitrate, int seconds);
		};

		static inline const QMap<Extension, ExtensionData> Data {
			{ Extension::MP3, { MetadataType::MPEG, "mp3", "", "-c:v copy", false, CalculateBitrateFileSize } },
			{ Extension::M4A, { MetadataType::MP4, "m4a", "", "", false, CalculateBitrateFileSize } },
			{ Extension::AAC, { MetadataType::MP4, "aac", "", "-acodec copy", false, CalculateBitrateFileSize } },
			{ Extension::WAV, { MetadataType::RIFF, "wav", "s16le", "-acodec pcm_s16le -ar 44100 -ac 2", true, CalculateWAVFileSize } }
		};
};

#endif