#ifndef ARTISTDATA_H
#define ARTISTDATA_H

#include <string>

#include <iostream>

struct ArtistData
{
    std::string Id = "";
    std::string Name = "";

    void Print()
    {
        std::cout << Id << std::endl;
        std::cout << Name << std::endl;
    }
};

#endif