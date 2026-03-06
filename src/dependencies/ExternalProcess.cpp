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

#ifdef UNIX
    // Dont relay outputs to stdout
    command += " 2>&1";
#endif

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
    SECURITY_ATTRIBUTES securityAttributes; 
    securityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
    securityAttributes.bInheritHandle = TRUE; 
    securityAttributes.lpSecurityDescriptor = NULL; 

    HANDLE hReadPipe;
    HANDLE hWritePipe;
    if (!CreatePipe(&hReadPipe, &hWritePipe, &securityAttributes, 0)) {
        std::cout << "Failed to create process pipe" << std::endl;
        return "";
    }

    SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOA startupInfo;
    ZeroMemory(&startupInfo, sizeof(STARTUPINFOA));
    startupInfo.cb = sizeof(STARTUPINFOA);
    startupInfo.hStdOutput = hWritePipe;
    startupInfo.hStdError = hWritePipe;
    startupInfo.dwFlags |= STARTF_USESTDHANDLES;

    PROCESS_INFORMATION processInfo;
    ZeroMemory(&processInfo, sizeof(PROCESS_INFORMATION));

    std::string command = GetCommand();
    bool processStarted = CreateProcessA(
        NULL,
        command.data(),
        NULL,
        NULL,
        TRUE,
        CREATE_NO_WINDOW,
        NULL,
        NULL,
        &startupInfo,
        &processInfo
    );

    CloseHandle(hWritePipe);

    if (!processStarted) {
        CloseHandle(hReadPipe);

        std::cout << "Process not started" << std::endl;
        return "";
    }

    std::string output = "";
    char buffer[256];
    DWORD dwBytesRead;

    std::string lineBuffer = "";

    while (ReadFile(hReadPipe, buffer, sizeof(buffer) - 1, &dwBytesRead, NULL) && dwBytesRead > 0) {
        buffer[dwBytesRead] = '\0';
        lineBuffer += buffer;

        // Can return multiple lines, or not a complete line
        size_t newLinePos;
        while ((newLinePos = lineBuffer.find_first_of("\r\n")) != std::string::npos) {
            std::string line = lineBuffer.substr(0, newLinePos);
            lineBuffer = lineBuffer.substr(newLinePos + 1);

            if (!line.empty() && lineBuffer[0] == '\n')
                lineBuffer = lineBuffer.substr(1);

            if (line.empty())
                continue;

            output += line;
            if (lineAvailableCallback != nullptr)
                lineAvailableCallback(line);
        }
    }

    // Handle any extra output after last newline
    if (!lineBuffer.empty()) {
        output += lineBuffer;
        if (lineAvailableCallback != nullptr)
                lineAvailableCallback(lineBuffer);
    }

    WaitForSingleObject(processInfo.hProcess, INFINITE);

    CloseHandle(hReadPipe);
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);

    return output;
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