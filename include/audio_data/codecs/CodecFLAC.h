#ifndef CODECFLAC_H
#define CODECFLAC_H

#include "ICodec.h"

#include <taglib/flacfile.h>

class CodecFLAC : public ICodec
{
    public:
        EExtension               GetExtension()              const override { return EExtension::FLAC; };
        EMetadataType            GetMetadataType()           const override { return EMetadataType::XIPH; };
        std::string              GetString()                 const override { return "flac"; };
        std::vector<std::string> GetFfmpegConversionParams() const override {
            std::vector<std::string> params = { "-acodec", "flac", "-ac", "2" };

            switch (Config::LosslessFormats[EExtension::FLAC]) {
                case ELosslessCodecFormat::B16H44100:
                    params.push_back("-af");
                    params.push_back("aformat=s16:44100");
                    break;
                case ELosslessCodecFormat::B16H48000:
                    params.push_back("-af");
                    params.push_back("aformat=s16:48000");
                    break;
            }

            return params;
        };
        
        BitrateDetails GetBitrateDetails() const override { 
            return BitrateDetails {
                0, 0, true,
                0, 0, 0,
                0, 0, 0
            };
        }

        bool SetCoverArt(const TagLib::FileRef& fileRef, const TagLib::ByteVector& imageBytes) const override {
            TagLib::FLAC::File* flacFile = dynamic_cast<TagLib::FLAC::File*>(fileRef.file());
			TagLib::FLAC::Picture* coverArt = new TagLib::FLAC::Picture();
			coverArt->setData(imageBytes);
			coverArt->setMimeType("image/png");
			coverArt->setType(TagLib::FLAC::Picture::Type::FrontCover);
			flacFile->addPicture(coverArt);

            return true;
        }

        TagLib::Tag* GetFileTag(const TagLib::FileRef& fileRef) const override {
            return dynamic_cast<TagLib::FLAC::File*>(fileRef.file())->xiphComment(true);
        }
    };

#endif