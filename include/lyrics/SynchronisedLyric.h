#ifndef SYNCHRONISEDLYRIC_H
#define SYNCHRONISEDLYRIC_H

#include <string>

struct SynchronisedLyric
{
    int StartMs;
    int EndMs;

    std::string Lyric;
};

#endif