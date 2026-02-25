#ifndef SYNCHRONISEDLYRIC_H
#define SYNCHRONISEDLYRIC_H

#include <string>

struct SynchronisedLyric
{
    int StartMs;
    int EndMs;

    std::string Lyric;

    SynchronisedLyric(const int& startMs, const int& endMs, const std::string& lyric)
        : StartMs(startMs), EndMs(endMs), Lyric(lyric) {}
};

#endif