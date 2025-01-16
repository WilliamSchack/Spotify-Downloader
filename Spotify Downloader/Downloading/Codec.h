#ifndef CODEC_H
#define CODEC_H

#include <QString>
#include <QMap>

#include <taglib/fileref.h>

#include <taglib/mpegfile.h>
#include <taglib/mp4file.h>
#include <taglib/wavfile.h>
#include <taglib/flacfile.h>
#include <taglib/vorbisfile.h>

class Codec {
	private:
		// File size in MB
		static float CalculateBitrateFileSize(int bitrate, int seconds) {
			return (((float)bitrate * (float)seconds) / 8) / 1024; // Bitrate * Seconds
		}

		static float CalculatePCMFileSize(int bitrate, int seconds) {
			return (44.1 * (16 / 8) * 2 * (float)seconds) / 1024; // Sample Rate * (Bits Per Sample / 8) * Channels * Seconds
		}

		// Get tags for metadata
		static TagLib::Tag* GetMP3FileTag(TagLib::FileRef fileRef) {
			return dynamic_cast<TagLib::MPEG::File*>(fileRef.file())->ID3v2Tag(true);
		}
		
		static TagLib::Tag* GetM4AFileTag(TagLib::FileRef fileRef) {
			return dynamic_cast<TagLib::MP4::File*>(fileRef.file())->tag();
		}

		static TagLib::Tag* GetOGGFileTag(TagLib::FileRef fileRef) {
			return dynamic_cast<TagLib::Ogg::Vorbis::File*>(fileRef.file())->tag();
		}

		static TagLib::Tag* GetWAVFileTag(TagLib::FileRef fileRef) {
			return dynamic_cast<TagLib::RIFF::WAV::File*>(fileRef.file())->InfoTag();
		}

		static TagLib::Tag* GetFLACFileTag(TagLib::FileRef fileRef) {
			return dynamic_cast<TagLib::FLAC::File*>(fileRef.file())->xiphComment(true);
		}
	public:
		// Planning to add more, just testing some different codecs for now
		enum class Extension {
			M4A,
			MP3,
			OGG,
			WAV,
			FLAC
		};

		enum class MetadataType {
			ID3V2,
			MP4,
			RIFF,	// Not all programs support RIFF chunks
			XIPH
		};

		struct ExtensionData {
			MetadataType Type;	// How the metadata is handled
			QString String;		// String used at end of files

			QString FFMPEGConversionParams;	// If codec needs to be converted, use these parameters

			int DefaultBitrate;
			bool LockedBitrate;

			// For tooltip and setting max bitrate in ui
			int BitrateLowQuality;
			int BitrateGoodQuality;
			int BitrateHighQuality;

			// For details field below file size calculation
			QString CodecDetails;

			float (*CalculateFileSize)(int bitrate, int seconds);

			// Used to get tags without needing to modify Song.cpp for each codec
			TagLib::Tag* (*GetFileTag)(TagLib::FileRef fileRef);
		};

		static inline const QMap<Extension, ExtensionData> Data {
			{ Extension::M4A, { MetadataType::MP4, "m4a", "", 128, false, 64, 96, 128, "MP4 Tags", CalculateBitrateFileSize, GetM4AFileTag}},
			{ Extension::MP3, { MetadataType::ID3V2, "mp3", "-c:v copy", 192, false, 128, 192, 256, "Converting M4A > MP3 || ID3v2 Tags", CalculateBitrateFileSize, GetMP3FileTag}},
			{ Extension::OGG, { MetadataType::XIPH, "ogg", "-acodec libvorbis", 128, false, 64, 96, 128, "Converting M4A > OGG Vorbis || XIPH Tags", CalculateBitrateFileSize, GetOGGFileTag } },
			{ Extension::WAV, { MetadataType::RIFF, "wav", "-acodec pcm_s16le -ar 44100 -ac 2", 0, true, 0, 0, 0, R"(Converting M4A > WAV <span style="color:rgb(255, 0, 0);">(Lossy)</span> || 44.1kHz, 16-bit || RIFF Tags)", CalculatePCMFileSize, GetWAVFileTag } },
			{ Extension::FLAC, { MetadataType::XIPH, "flac", "-c:a flac -af aformat=s16:44100 -ac 2", 0, true, 0, 0, 0, R"(Converting M4A > FLAC <span style="color:rgb(255, 0, 0);">(Lossy)</span> || 44.1kHz, 16-bit || XIPH Tags)", CalculatePCMFileSize, GetFLACFileTag } }
		};
};

#endif