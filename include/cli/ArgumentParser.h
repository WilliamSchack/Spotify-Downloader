#ifndef ARGUMENTPARSER_H
#define ARGUMENTPARSER_H

#include "DownloadManager.h"

#include <string>

// Very basic arg parser, only whipped up quick since im using it for another app
// It has barely any functions at the moment, it will be expanded when the gui is finished

class ArgumentParser
{
    public:
        static bool Parse(const int& argc, const char** argv);
};

#endif