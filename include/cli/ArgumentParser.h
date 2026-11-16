#ifndef ARGUMENTPARSER_H
#define ARGUMENTPARSER_H

#include "Logger.h"
#include "DownloadManager.h"
#include "MetadataManager.h"

#include "Ytdlp.h"
#include "Ffmpeg.h"

#include <string>

// Very basic arg parser, only whipped up quick since im using it for another app
// It has barely any functions at the moment, it will be expanded when the gui is finished

class ArgumentParser
{
    public:
        static bool ParseBeforeGUI(int argc, char** argv);
        static bool ParseAfterGUI(int argc, char** argv);
};

#endif