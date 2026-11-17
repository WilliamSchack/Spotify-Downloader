#ifndef FFMPEGAUDIODETAILS_H
#define FFMPEGAUDIODETAILS_H

struct FfmpegAudioDetails
{
    unsigned int DurationMilliseconds = 0;
    unsigned int DurationSeconds = 0;
    unsigned int Bitrate = 0;

    float MeanDB = 0;
    float MaxDB = 0;

    float IntegratedLoudnessLUFS = 0;
    float IntegratedLoudnessThresholdLUFS = 0;

    float LoudnessRangeLUFS = 0;
    float LoudnessRangeThresholdLUFS = 0;
    float LoudnessRangeLowLUFS = 0;
    float LoudnessRangeHighLUFS = 0;

    float TruePeakDBFS = 0;
};

#endif