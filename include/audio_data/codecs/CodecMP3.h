#ifndef CODECMP3_H
#define CODECMP3_H

#include "ICodec.h"

class CodecMP3 : public ICodec
{
    public:
        EExtension     GetExtension()              const override { return EExtension::MP3; };
        EMetadataType  GetMetadataType()           const override { return EMetadataType::ID3V2; };
        std::string    GetString()                 const override { return "mp3"; };
        std::string    GetFfmpegConversionParams() const override { return "-acodec libmp3lame"; };
        BitrateDetails GetBitrateDetails()         const override { 
            return BitrateDetails {
                192, 320, false,
                128, 160, 192,
                192, 256, 320
            };
        }
};

#endif