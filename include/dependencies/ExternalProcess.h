#ifndef EXTERNALPROCESS_H
#define EXTERNALPROCESS_H

#include "ExternalProcessResult.h"
#include "FileUtils.h"
#include "StringUtils.h"

#include <vector>
#include <functional>
#include <chrono>

#ifdef WIN32
#include <windows.h>
#include <atomic>
#include <thread>
#else
#include <sys/wait.h>
#include <cerrno>
#include <poll.h>
#endif

class ExternalProcess
{
    public:
        ExternalProcess(const std::filesystem::path& path);
        static ExternalProcess GetRelativeProcess(const std::filesystem::path& relativePath);

        void AddArgument(const std::string& arg);
        void AddArgument(const std::string& arg, const std::string& value);

        ExternalProcessResult Execute(const int& timeoutMs = 0, std::function<void(std::string)> lineAvailableCallback = nullptr);
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