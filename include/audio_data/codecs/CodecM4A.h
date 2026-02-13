#ifndef CODECM4A_H
#define CODECM4A_H

#include "ICodec.h"

class CodecM4A : public ICodec
{
    public:
        EExtension     GetExtension()              const override { return EExtension::M4A; };
        EMetadataType  GetMetadataType()           const override { return EMetadataType::MP4; };
        std::string    GetString()                 const override { return "m4a"; };
        std::string    GetFfmpegConversionParams() const override { return "-acodec aac"; };
        BitrateDetails GetBitrateDetails()         const override { return BitrateDetails(); }; // Default values
};

#endif