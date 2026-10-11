#ifndef LYRICSFILE_H
#define LYRICSFILE_H

#include "Lyrics.h"
#include "TrackData.h"
#include "FileUtils.h"
#include "MetadataManager.h"

#include <iostream>
#include <fstream>

class LyricsFile
{
    public:
        static bool Create(const std::string& outputPathNoExtension, const TrackData& trackData, const Lyrics& lyrics);
};

#endif