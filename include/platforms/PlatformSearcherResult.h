#ifndef SEARCHERRESULT_H
#define SEARCHERRESULT_H

#include "TrackData.h"

struct PlatformSearcherResult
{
    TrackData Data;
    double Confidence = 0.0;

    PlatformSearcherResult() : Data(EPlatform::Unknown) {}
};

#endif