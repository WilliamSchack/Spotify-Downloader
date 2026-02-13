#include "CodecFactory.h"

std::unique_ptr<ICodec> CodecFactory::Create(const EExtension& extension)
{
    switch (extension) {
        case EExtension::M4A:  return std::make_unique<CodecM4A>();
        case EExtension::AAC:  return std::make_unique<CodecAAC>();
        case EExtension::MP3:  return std::make_unique<CodecMP3>();
        case EExtension::OGG:  return std::make_unique<CodecOGG>();
        case EExtension::WAV:  return std::make_unique<CodecWAV>();
        case EExtension::FLAC: return std::make_unique<CodecFLAC>();
        case EExtension::WEBM: return std::make_unique<CodecWEBM>();
    }

    return nullptr;
}

std::unique_ptr<ICodec> CodecFactory::Create(const std::string& extension)
{
    if (extension.empty())
        return nullptr;

    std::string extensionLower = extension;
    StringUtils::ToLower(extensionLower);

    // Remove dot at the front if it has one
    if (StringUtils::StartsWith(extensionLower, "."))
        StringUtils::RemoveChar(extensionLower, '.');

    if (extensionLower == "m4a")  return Create(EExtension::M4A);
    if (extensionLower == "aac")  return Create(EExtension::AAC);
    if (extensionLower == "mp3")  return Create(EExtension::MP3);
    if (extensionLower == "ogg")  return Create(EExtension::OGG);
    if (extensionLower == "wav")  return Create(EExtension::WAV);
    if (extensionLower == "flac") return Create(EExtension::FLAC);
    if (extensionLower == "webm") return Create(EExtension::WEBM);
    return nullptr;
}