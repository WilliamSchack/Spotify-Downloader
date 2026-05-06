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

#ifdef WIN32
        void AddArgument(const std::wstring& arg);
        void AddArgument(const std::wstring& arg, const std::wstring& value);
#endif
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
#ifdef WIN32
        std::vector<std::wstring> _args;
#else
        std::vector<std::string> _args;
#endif
};

#endif