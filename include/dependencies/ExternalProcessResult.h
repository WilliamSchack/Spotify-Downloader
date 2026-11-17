#ifndef EXTERNALPROCESSRESULT_H
#define EXTERNALPROCESSRESULT_H

#include <string>

struct ExternalProcessResult
{
    std::string Output = "";
    bool TimedOut = false;
};

#endif