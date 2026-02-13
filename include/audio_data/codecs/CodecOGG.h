#ifndef CODECOGG_H
#define CODECOGG_H

#include "ICodec.h"

class CodecOGG : public ICodec
{
    public:
        EExtension     GetExtension()              const override { return EExtension::OGG; };
        EMetadataType  GetMetadataType()           const override { return EMetadataType::XIPH; };
        std::string    GetString()                 const override { return "ogg"; };
        std::string    GetFfmpegConversionParams() const override { return "-acodec libvorbis"; };
        BitrateDetails GetBitrateDetails()         const override { return BitrateDetails(); }; // Default values
};

#endif