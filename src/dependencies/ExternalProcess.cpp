#include "ExternalProcess.h"

ExternalProcess::ExternalProcess(const std::filesystem::path& path)
    : _path(path) {}

ExternalProcess ExternalProcess::GetRelativeProcess(const std::filesystem::path& relativePath)
{
    std::filesystem::path executablePath = FileUtils::GetExecutablePath();
    std::filesystem::path processPath = executablePath.parent_path() / relativePath;

    return ExternalProcess(processPath);
}

#ifdef WIN32
std::wstring ExternalProcess::GetCommand()
{
    std::wstring command = L"\"" + _path.wstring() + L"\"";

    int argsSize = _args.size();
    for (const std::string& arg : _args) {
        command += L" ";
        command += StringUtils::ToWString(arg);
    }

    return command;
}
#else
std::string ExternalProcess::GetCommand()
{
    std::string command = "\"" + _path.string() + "\"";

    int argsSize = _args.size();
    for (const std::string& arg : _args) {
        command += " ";
        command += arg;
    }

    return command;
}
#endif

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
#ifdef WIN32
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

    STARTUPINFOW startupInfo;
    ZeroMemory(&startupInfo, sizeof(STARTUPINFOW));
    startupInfo.cb = sizeof(STARTUPINFOW);
    startupInfo.hStdOutput = hWritePipe;
    startupInfo.hStdError = hWritePipe;
    startupInfo.dwFlags |= STARTF_USESTDHANDLES;

    PROCESS_INFORMATION processInfo;
    ZeroMemory(&processInfo, sizeof(PROCESS_INFORMATION));

    std::wstring command = GetCommand();
    bool processStarted = CreateProcessW(
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
    std::string lineBuffer = "";
    char buffer[256];
    DWORD dwBytesRead;

    while (ReadFile(hReadPipe, buffer, sizeof(buffer) - 1, &dwBytesRead, NULL) && dwBytesRead > 0) {
        buffer[dwBytesRead] = '\0';
        lineBuffer += buffer;

        // Can return multiple lines, or not a complete line
        size_t newLinePos;
        while ((newLinePos = lineBuffer.find_first_of("\r\n")) != std::string::npos) {
            std::string line = lineBuffer.substr(0, newLinePos);
            lineBuffer.erase(0, newLinePos + 1);

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
    std::string command = GetCommand();

    int pipeFd[2];
    if (pipe(pipeFd) != 0) {
        perror("Failed to open process");
        return "";
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        close(pipeFd[0]);
        close(pipeFd[1]);
        return "";
    }

    if (pid == 0) {
        setpgid(0, 0);
        dup2(pipeFd[1], STDOUT_FILENO);
        dup2(pipeFd[1], STDERR_FILENO);
        close(pipeFd[0]);
        close(pipeFd[1]);

        execl("/bin/sh", "sh", "-c", command.c_str(), (char*)nullptr);
        _exit(127);
    }
    close(pipeFd[1]);

    std::string output = "";
    std::string lineBuffer = "";
    char buffer[256];

    while (true) {
        ssize_t n = read(pipeFd[0], buffer, sizeof(buffer));
        if (n < 0) {
            if (errno == EINTR)
                continue;
            
            perror("Failed to read process");
            break;
        }
        if (n == 0)
            break;

        lineBuffer.append(buffer, n);

        size_t newLinePos;
        while ((newLinePos = lineBuffer.find('\n')) != std::string::npos) {
            std::string line = lineBuffer.substr(0, newLinePos);
            lineBuffer.erase(0, newLinePos + 1);

            if (line.empty())
                continue;

            output += line;
            if (lineAvailableCallback != nullptr)
                lineAvailableCallback(line);
        }
    }

    if (!lineBuffer.empty()) {
        output += lineBuffer;
        if (lineAvailableCallback != nullptr)
            lineAvailableCallback(lineBuffer);
    }

    close(pipeFd[0]);
    waitpid(pid, nullptr, 0);
    return output;
#endif
}