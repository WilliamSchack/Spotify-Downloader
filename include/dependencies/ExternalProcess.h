#ifndef EXTERNALPROCESS_H
#define EXTERNALPROCESS_H

#include "FileUtils.h"
#include "StringUtils.h"

#include <vector>
#include <functional>

#ifdef WIN32
#include <windows.h>
#endif

class ExternalProcess
{
    public:
        ExternalProcess(const std::filesystem::path& path);
        static ExternalProcess GetRelativeProcess(const std::filesystem::path& relativePath);

        void AddArgument(const std::string& arg);
        void AddArgument(const std::string& arg, const std::string& value);

        std::string Execute(std::function<void(std::string)> lineAvailableCallback = nullptr);
    private:
#ifdef WIN32
        std::wstring GetCommand();
#else
        std::string GetCommand();
#endif
    private:
        std::filesystem::path _path;
        std::vector<std::string> _args;
};

#endif