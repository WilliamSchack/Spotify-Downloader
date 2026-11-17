#ifndef EIPVERSION_H
#define EIPVERSION_H

#include <vector>
#include <string>

enum class EIPVersion
{
    Automatic,
    IPv4,
    IPv6
};

static inline const std::vector<std::string> EIPVERSION_NAMES = {
    "Automatic",
    "IPv4",
    "IPv6"
};


#endif