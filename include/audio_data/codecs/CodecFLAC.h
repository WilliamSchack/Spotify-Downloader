#ifndef CODECFLAC_H
#define CODECFLAC_H

#include "ICodec.h"

class CodecFLAC : public ICodec
{
    public:
        EExtension     GetExtension()              const override { return EExtension::FLAC; };
        EMetadataType  GetMetadataType()           const override { return EMetadataType::XIPH; };
        std::string    GetString()                 const override { return "flac"; };
        std::string    GetFfmpegConversionParams() const override { return "-acodec flac -af aformat=s16:44100 -ac 2"; };
        BitrateDetails GetBitrateDetails()         const override { 
            return BitrateDetails {
                0, 0, true,
                0, 0, 0,
                0, 0, 0
            };
        }
};

#endif