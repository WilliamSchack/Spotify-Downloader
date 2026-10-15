#ifndef IPLATFORMDATA_H
#define IPLATFORMDATA_H

#include <filesystem>

class IPlatformData
{
    public:
        virtual ~IPlatformData() = default;

        virtual std::filesystem::path GetCookiePath() const = 0;
};

#endif