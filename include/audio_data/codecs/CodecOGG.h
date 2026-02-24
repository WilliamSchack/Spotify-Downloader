#ifndef CODECOGG_H
#define CODECOGG_H

#include "ICodec.h"

#include <taglib/vorbisfile.h>

class CodecOGG : public ICodec
{
    public:
        EExtension     GetExtension()              const override { return EExtension::OGG; };
        EMetadataType  GetMetadataType()           const override { return EMetadataType::XIPH; };
        std::string    GetString()                 const override { return "ogg"; };
        std::string    GetFfmpegConversionParams() const override { return "-acodec libvorbis"; };
        BitrateDetails GetBitrateDetails()         const override { return BitrateDetails(); }; // Default values

        bool SetCoverArt(const TagLib::FileRef& fileRef, const TagLib::ByteVector& imageBytes) const override { return false; }
        TagLib::Tag* GetFileTag(const TagLib::FileRef& fileRef) const override {
            return dynamic_cast<TagLib::Ogg::Vorbis::File*>(fileRef.file())->tag();
        }
};

#endif