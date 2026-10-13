#ifndef PLAYLISTFILEFACTORY_H
#define PLAYLISTFILEFACTORY_H

#include "EPlaylistFileType.h"
#include "M3UFile.h"
#include "XSPFFile.h"
#include "PLSFile.h"

#include <memory>

class PlaylistFileFactory
{
    public:
        static std::unique_ptr<IPlaylistFile> Create(const EPlaylistFileType& fileType);
};

#endif