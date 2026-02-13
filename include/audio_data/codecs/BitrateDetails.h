#ifndef BITRATEDETAILS_H
#define BITRATEDETAILS_H

struct BitrateDetails
{
    unsigned int MaxBitrate = 128;
    unsigned int MaxBitratePremium = 256;
    bool LockedBirtate = false;
    
    unsigned int BitrateLowQuality = 64;
    unsigned int BitrateGoodQuality = 96;
    unsigned int BitrateHighQuality = 128;

    unsigned int BitrateLowQualityPremium = 128;
    unsigned int BitrateGoodQualityPremium = 192;
    unsigned int BitrateHighQualityPremium = 256;
};

#endif