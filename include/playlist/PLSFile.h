#ifndef PLSFILE_H
#define PLSFILE_H

#include "IPlaylistFile.h"

class PLSFile : public IPlaylistFile
{
    public:
        std::string GetExtension();
        std::string WriteHeader();
        std::string WriteFile(const PlaylistFileTrack& track);
        std::string WriteFooter();
    private:
        int _currentFile = 1;
};

#endif