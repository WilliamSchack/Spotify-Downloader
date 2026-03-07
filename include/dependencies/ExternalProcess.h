#ifndef EXTERNALPROCESS_H
#define EXTERNALPROCESS_H

#include "FileUtils.h"
#include "StringUtils.h"

#include <vector>
#include <functional>

#if WIN32
#include <windows.h>
#endif

class ExternalProcess
{
    public:
        ExternalProcess(const std::filesystem::path& path);
        static ExternalProcess GetRelativeProcess(const std::filesystem::path& relativePath);

        void AddArgument(const std::wstring& arg);
        void AddArgument(const std::wstring& arg, const std::wstring& value);
        void AddArgument(const std::string& arg);
        void AddArgument(const std::string& arg, const std::string& value);

        std::string Execute(std::function<void(std::string)> lineAvailableCallback = nullptr);
    private:
        std::wstring GetCommand();
    private:
        std::filesystem::path _path;
        std::vector<std::wstring> _args;
};

#endif