#ifndef PLSFILE_H
#define PLSFILE_H

#include "IPlaylistFile.h"

class PLSFile : public IPlaylistFile
{
    public:
        std::string GetExtension() override;
        std::string WriteHeader() override;
        std::string WriteFile(const PlaylistFileTrack& track) override;
        std::string WriteFooter() override;
    private:
        int _currentFile = 1;
};

#endif