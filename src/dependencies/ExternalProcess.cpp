#include "ExternalProcess.h"

ExternalProcess::ExternalProcess(const std::filesystem::path& path)
    : _path(path) {}

ExternalProcess ExternalProcess::GetRelativeProcess(const std::filesystem::path& relativePath)
{
    std::filesystem::path executablePath = FileUtils::GetExecutablePath();
    std::filesystem::path processPath = executablePath.parent_path() / relativePath;

    return ExternalProcess(processPath);
}

std::string ExternalProcess::GetCommand()
{
    std::string command = "\"" + _path.string() + "\"";

    int argsSize = _args.size();
    for (int i = 0; i < argsSize; i++) {
        command += " ";
        command += _args[i];
    }

    // Dont relay outputs to stdout
    command += " 2>&1";

    return command;
}

void ExternalProcess::AddArgument(const std::string& arg)
{
    _args.push_back(arg);
}

void ExternalProcess::AddArgument(const std::string& arg, const std::string& value)
{
    AddArgument(arg + " " + value);
}

std::string ExternalProcess::Execute(std::function<void(std::string)> lineAvailableCallback)
{
#if WIN32
    STARTUPINFO info;
    PROCESS_INFORMATION processInfo;

    return "";
#else
    FILE* pipe = popen(GetCommand().c_str(), "r");
    if (pipe == NULL) {
        perror("Failed to open process");
        return "";
    }

    std::string output = "";
    char buffer[256];
    
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        std::string line = buffer;

        // Remove ending new line
        if (!line.empty() && line.back() == '\n')
            line = StringUtils::RemoveLast(line);

        output += line;

        if (lineAvailableCallback != nullptr)
            lineAvailableCallback(line);
    }
    
    int exitStatus = pclose(pipe);
    if (exitStatus == -1) {
        perror("Failed to close process");
        return "";
    }

    return output;
#endif
}