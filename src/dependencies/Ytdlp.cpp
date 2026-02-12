#include "Ytdlp.h"

std::string Ytdlp::GetVersion()
{
    std::filesystem::path executablePath = FileUtils::GetExecutablePath();
    std::filesystem::path ytdlpPath = executablePath.parent_path() / YTDLP_PATH_RELATIVE;

    std::string testCommand = ytdlpPath.string() + " --version";

    FILE* pipe = popen(testCommand.c_str(), "r");
    if (pipe == NULL) {
        perror("Failed to open ytdlp");
        return "";
    }

    std::string output = "";
    char buffer[256];
    
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        output += buffer;
    }
    
    int exitStatus = pclose(pipe);
    if (exitStatus == -1) {
        perror("Failed to close ytdlp");
        return "";
    }

    return output;
}