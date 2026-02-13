#ifndef ICODEC_H
#define ICODEC_H

#include "BitrateDetails.h"
#include "EExtension.h"
#include "EMetadataType.h"

#include <string>

class ICodec
{
    public:
        virtual ~ICodec() = default;

        virtual EExtension GetExtension() const = 0;
        virtual EMetadataType GetMetadataType() const = 0;
        virtual std::string GetString() const = 0;
        virtual std::string GetFfmpegConversionParams() const = 0;
        virtual BitrateDetails GetBitrateDetails() const = 0;
};

#endif