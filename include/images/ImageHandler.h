#ifndef IMAGEHANDLER_H
#define IMAGEHANDLER_H

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#include "Image.h"

#include <filesystem>

class ImageHandler
{
    public:
        static bool SavePng(const std::filesystem::path& path, const Image& image);
        static bool SaveJpg(const std::filesystem::path& path, const Image& image, const int& quality = 100);
        static Image LoadImage(const std::filesystem::path& path);
};

#endif