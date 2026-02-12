#include "Process.h"

Process::Process(const std::filesystem::path& path)
    : _path(path) {}

Process Process::GetRelativeProcess(const std::filesystem::path& relativePath)
{
    std::filesystem::path executablePath = FileUtils::GetExecutablePath();
    std::filesystem::path processPath = executablePath.parent_path() / relativePath;

    return Process(processPath);
}

std::string Process::GetCommand()
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

void Process::AddArgument(const std::string& arg)
{
    _args.push_back(arg);
}

void Process::AddArgument(const std::string& arg, const std::string& value)
{
    AddArgument(arg + " " + value);
}

std::string Process::Execute(std::function<void(std::string)> lineAvailableCallback)
{
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
}