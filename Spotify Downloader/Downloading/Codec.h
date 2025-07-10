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
		static TagLib::Tag* GetM4AFileTag(TagLib::FileRef fileRef) {
			return dynamic_cast<TagLib::MP4::File*>(fileRef.file())->tag();
		}

		static TagLib::Tag* GetMP3FileTag(TagLib::FileRef fileRef) {
			return dynamic_cast<TagLib::MPEG::File*>(fileRef.file())->ID3v2Tag(true);
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

		static void SetFLACCoverArt(TagLib::FileRef fileRef, QByteArray imageBytes) {
			TagLib::FLAC::File* flacFile = dynamic_cast<TagLib::FLAC::File*>(fileRef.file());
			TagLib::FLAC::Picture* coverArt = new TagLib::FLAC::Picture();
			coverArt->setData(TagLib::ByteVector(imageBytes.data(), imageBytes.count()));
			coverArt->setMimeType("image/png");
			coverArt->setType(TagLib::FLAC::Picture::Type::FrontCover);
			flacFile->addPicture(coverArt);
		}
	public:
		// Planning to add more, just testing some different codecs for now
		enum class Extension {
			M4A,
			AAC,	// Cannot store metadata, raw audio stream
			MP3,
			OGG,
			WAV,
			FLAC
		};

		enum class MetadataType {
			ID3V2,
			MP4,
			RIFF,	// Not all programs support RIFF chunks
			XIPH,
			NONE
		};

		struct ExtensionData {
			MetadataType Type;	// How the metadata is handled
			void (*CoverArtOverride)(TagLib::FileRef fileRef, QByteArray imageBytes); // If the codec requires different setting for the cover art
			QString String;		// String used at end of files

			QString FFMPEGConversionParams;	// If codec needs to be converted, use these parameters

			int MaxBitrate;
			int MaxBitratePremium;
			bool LockedBitrate;

			// For tooltip and setting max bitrate in ui
			int BitrateLowQuality;
			int BitrateGoodQuality;
			int BitrateHighQuality;

			int BitrateLowQualityPremium;
			int BitrateGoodQualityPremium;
			int BitrateHighQualityPremium;

			// For details field below file size calculation
			QString CodecDetails;

			float (*CalculateFileSize)(int bitrate, int seconds);

			// Used to get tags without needing to modify Song.cpp for each codec
			TagLib::Tag* (*GetFileTag)(TagLib::FileRef fileRef);
		};

		static inline const QMap<Extension, ExtensionData> Data {
			{ Extension::M4A,  { MetadataType::MP4, NULL, "m4a", "-acodec aac", 128, 256, false, 64, 96, 128, 128, 192, 256, "", CalculateBitrateFileSize, GetM4AFileTag}},
			{ Extension::AAC,  { MetadataType::NONE, NULL, "aac", "-acodec aac", 128, 256, false, 64, 96, 128, 128, 192, 256, R"(Converting M4A > AAC || Audio Only, <span style="color:rgb(255, 0, 0); ">No Metadata</span>)", CalculateBitrateFileSize, NULL}},
			{ Extension::MP3,  { MetadataType::ID3V2, NULL, "mp3", "-acodec libmp3lame", 192, 320, false, 128, 160, 192, 192, 256, 320, "Converting M4A > MP3 || ID3v2 Tags", CalculateBitrateFileSize, GetMP3FileTag}},
			{ Extension::OGG,  { MetadataType::XIPH, NULL, "ogg", "-acodec libvorbis", 128, 256, false, 64, 96, 128, 128, 192, 256, R"(Converting M4A > OGG Vorbis || XIPH Tags, <span style="color:rgb(255, 0, 0); ">Minimal Cover Art Support</span>)", CalculateBitrateFileSize, GetOGGFileTag } },
			{ Extension::WAV,  { MetadataType::RIFF, NULL, "wav", "-acodec pcm_s16le -af aformat=s16:44100 -ac 2", 0, 0, true, 0, 0, 0, 0, 0, 0, R"(Converting M4A > WAV <span style="color:rgb(255, 0, 0);">(Lossy)</span> || 44.1kHz, 16-bit || RIFF Tags, <span style="color:rgb(255, 0, 0);">No Cover Art</span>)", CalculatePCMFileSize, GetWAVFileTag } },
			{ Extension::FLAC, { MetadataType::XIPH, SetFLACCoverArt, "flac", "-acodec flac -af aformat=s16:44100 -ac 2", 0, 0, true, 0, 0, 0, 0, 0, 0, R"(Converting M4A > FLAC <span style="color:rgb(255, 0, 0);">(Lossy)</span> || 44.1kHz, 16-bit || XIPH Tags)", CalculatePCMFileSize, GetFLACFileTag }}
		};
};

#endif