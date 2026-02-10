#ifndef ARTISTDATA_H
#define ARTISTDATA_H

#include "EPlatform.h"

#include <string>

#include <iostream>

struct ArtistData
{
    EPlatform Platform = EPlatform::Unknown;
    std::string Id = "";
    std::string Url = "";
    std::string Name = "";

    void Print()
    {
        std::cout << (int)Platform << std::endl;
        std::cout << Id << std::endl;
        std::cout << Url << std::endl;
        std::cout << Name << std::endl;
    }
};

#endif