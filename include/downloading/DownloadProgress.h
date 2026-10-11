#ifndef DOWNLOADPROGRESS_H
#define DOWNLOADPROGRESS_H

struct DownloadProgress
{
    float Progress = 0.0f;
    std::string Message = "";

    DownloadProgress(float progress, std::string message) : Progress(progress), Message(message) {}
};

#endif