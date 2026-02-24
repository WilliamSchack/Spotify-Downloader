#ifndef CODECAAC_H
#define CODECAAC_H

#include "ICodec.h"

class CodecAAC : public ICodec
{
    public:
        EExtension     GetExtension()              const override { return EExtension::AAC; };
        EMetadataType  GetMetadataType()           const override { return EMetadataType::None; };
        std::string    GetString()                 const override { return "aac"; };
        std::string    GetFfmpegConversionParams() const override { return "-acodec aac"; };
        BitrateDetails GetBitrateDetails()         const override { return BitrateDetails(); }; // Default values
        
        bool SetCoverArt(const TagLib::FileRef& fileRef, const TagLib::ByteVector& imageBytes) const override { return false; }
        TagLib::Tag* GetFileTag(const TagLib::FileRef& fileRef) const override { return nullptr; }
};

#endif