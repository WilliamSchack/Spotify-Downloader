#ifndef CODECFACTORY_H
#define CODECFACTORY_H

#include "CodecM4A.h"
#include "CodecAAC.h"
#include "CodecMP3.h"
#include "CodecOGG.h"
#include "CodecWAV.h"
#include "CodecFLAC.h"
#include "CodecWEBM.h"
#include "StringUtils.h"

#include <string>
#include <memory>

class CodecFactory
{
    public:
        static std::unique_ptr<ICodec> Create(EExtension extension);
        static std::unique_ptr<ICodec> Create(const std::string& extension);
};

#endif