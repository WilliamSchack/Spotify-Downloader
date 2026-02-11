#ifndef YOUTUBESEARCHRESULT_H
#define YOUTUBESEARCHRESULT_H

#include "EYoutubeCategory.h"
#include "TrackData.h"
#include "AlbumData.h"

#include <variant>

#include <iostream>

struct YoutubeSearchResult
{
    std::variant<std::monostate, TrackData, AlbumData> Data;

    EYoutubeCategory Category = EYoutubeCategory::None;
    std::string VideoType = "";
    std::string BrowseId = "";
    std::string Views = "";

    void Print() {
        std::cout << (int)Category << std::endl;
        std::cout << VideoType << std::endl;
        std::cout << BrowseId << std::endl;
        std::cout << Views << std::endl;
        
        std::cout << "DATA:" << std::endl;
        
        if (std::holds_alternative<TrackData>(Data)) {
            std::get<TrackData>(Data).Print();
        }

        if (std::holds_alternative<AlbumData>(Data)) {
            std::get<AlbumData>(Data).Print();
        }
    }
};

#endif