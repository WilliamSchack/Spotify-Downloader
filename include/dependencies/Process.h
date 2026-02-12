#ifndef PROCESS_H
#define PROCESS_H

#include "FileUtils.h"

#include <vector>

class Process
{
    public:
        Process(const std::filesystem::path& path);
        static Process GetRelativeProcess(const std::filesystem::path& relativePath);

        void AddArgument(const std::string& arg);
        void AddArgument(const std::string& arg, const std::string& value);

        std::string Execute();
    private:
        std::filesystem::path _path;
        std::vector<std::string> _args;
    private:
        std::string GetCommand();
};

#endif