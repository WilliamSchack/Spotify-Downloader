#include "Ytdlp.h"

std::string Ytdlp::GetVersion()
{
    Process process = Process::GetRelativeProcess(YTDLP_PATH_RELATIVE);
    process.AddArgument("--version");

    std::string output = process.Execute();
    return output;
}