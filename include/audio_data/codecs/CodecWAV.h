#ifndef CODECWAV_H
#define CODECWAV_H

#include "ICodec.h"
#include "Config.h"

#include <taglib/wavfile.h>

class CodecWAV : public ICodec
{
    public:
        EExtension               GetExtension()              const override { return EExtension::WAV; };
        EMetadataType            GetMetadataType()           const override { return EMetadataType::RIFF; };
        std::string              GetString()                 const override { return "wav"; };
        std::vector<std::string> GetFfmpegConversionParams() const override {
            std::vector<std::string> params = { "-ac", "2" };

            switch (Config::LosslessFormats[EExtension::WAV]) {
                case ELosslessCodecFormat::B16H44100:
                    params.push_back("-acodec");
                    params.push_back("pcm_s16le");
                    params.push_back("-af");
                    params.push_back("aformat=s16:44100");
                    break;
                case ELosslessCodecFormat::B16H48000:
                    params.push_back("-acodec");
                    params.push_back("pcm_s16le");
                    params.push_back("-af");
                    params.push_back("aformat=s16:48000");
                    break;
            }

            return params;
        };
        
        BitrateDetails GetBitrateDetails()         const override { 
            return BitrateDetails {
                0, 0, true,
                0, 0, 0,
                0, 0, 0
            };
        }

        bool SetCoverArt(const TagLib::FileRef& fileRef, const TagLib::ByteVector& imageBytes) const override { return false; }
        TagLib::Tag* GetFileTag(const TagLib::FileRef& fileRef) const override {
            return dynamic_cast<TagLib::RIFF::WAV::File*>(fileRef.file())->InfoTag();
        }
};

#endif