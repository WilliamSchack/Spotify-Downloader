#ifndef SEARCHERRESULT_H
#define SEARCHERRESULT_H

#include "TrackData.h"

struct PlatformSearcherResult
{
    TrackData Data;
    double Confidence;
};

#endif