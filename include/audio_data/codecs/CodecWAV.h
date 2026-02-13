#ifndef CODECWAV_H
#define CODECWAV_H

#include "ICodec.h"

class CodecWAV : public ICodec
{
    public:
        EExtension     GetExtension()              const override { return EExtension::WAV; };
        EMetadataType  GetMetadataType()           const override { return EMetadataType::RIFF; };
        std::string    GetString()                 const override { return "wav"; };
        std::string    GetFfmpegConversionParams() const override { return "-acodec pcm_s16le -af aformat=s16:44100 -ac 2"; };
        BitrateDetails GetBitrateDetails()         const override { 
            return BitrateDetails {
                0, 0, true,
                0, 0, 0,
                0, 0, 0
            };
        }
};

#endif