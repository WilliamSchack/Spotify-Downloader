#ifndef CODECWEBM_H
#define CODECWEBM_H

#include "ICodec.h"

// Isnt used as a target codec, only for checking against
class CodecWEBM : public ICodec
{
    public:
        EExtension     GetExtension()              const override { return EExtension::WEBM; };
        EMetadataType  GetMetadataType()           const override { return EMetadataType::None; };
        std::string    GetString()                 const override { return "webm"; };
        std::string    GetFfmpegConversionParams() const override { return ""; };
        BitrateDetails GetBitrateDetails()         const override { 
            return BitrateDetails {
                0, 0, false,
                0, 0, 0,
                0, 0, 0
            };
        }
};

#endif