#ifndef JSRUNTIME_H
#define JSRUNTIME_H

#include "ExternalProcess.h"
#include "NetworkRequest.h"
#include "ArchiveHandler.h"
#include "Paths.h"

#include <nlohmann/json.hpp>

// Only checks for and uses quickjs right now
// May check for more system installed ones in the future but this makes it easy

class JsRuntime
{
    public:
        static bool IsInstalled();
        static bool IsInstalledRelative();
        static bool Download();
        static std::filesystem::path GetPath();
    private:
        static std::string GetRelativePath();
};

#endif