#ifndef CODECWEBM_H
#define CODECWEBM_H

#include "ICodec.h"

#include <taglib/matroskafile.h>

class CodecWEBM : public ICodec
{
    public:
        EExtension               GetExtension()              const override { return EExtension::WEBM; };
        EMetadataType            GetMetadataType()           const override { return EMetadataType::Matroska; };
        std::string              GetString()                 const override { return "webm"; };
        std::vector<std::string> GetFfmpegConversionParams() const override { return { "-acodec", "libopus" }; };
        BitrateDetails           GetBitrateDetails()         const override { return BitrateDetails(); }; // Default values

        bool SetCoverArt(const TagLib::FileRef& fileRef, const TagLib::ByteVector& imageBytes) const override { return false; }
        TagLib::Tag* GetFileTag(const TagLib::FileRef& fileRef) const override {
            return dynamic_cast<TagLib::Matroska::File*>(fileRef.file())->tag();
        }
};

#endif