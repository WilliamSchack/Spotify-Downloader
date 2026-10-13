#include "PlaylistFileFactory.h"

std::unique_ptr<IPlaylistFile> PlaylistFileFactory::Create(const EPlaylistFileType& fileType)
{
    switch (fileType) {
        case EPlaylistFileType::M3U:  return std::make_unique<M3UFile>();
        case EPlaylistFileType::XSPF: return std::make_unique<XSPFFile>();
        case EPlaylistFileType::PLS:  return std::make_unique<PLSFile>();
    }

    return nullptr;
}