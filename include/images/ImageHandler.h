#ifndef IMAGEHANDLER_H
#define IMAGEHANDLER_H

#include "stb_image.h"
#include "stb_image_write.h"

#include "Image.h"
#include "NetworkRequest.h"
#include "StringUtils.h"

#include <iostream>
#include <filesystem>

class ImageHandler
{
    public:
        static std::string  GetImageFormatString(const EImageFormat& format);
        static EImageFormat GetImageFormat(const std::string& string);
        static EImageFormat GetImageFormat(const std::filesystem::path& path);
        static EImageFormat GetImageFormat(const NetworkResponse& response);

        static bool SaveImage(const std::filesystem::path& pathNoExtension, const Image& image);
        static bool SavePng(const std::filesystem::path& path, const Image& image);
        static bool SaveJpg(const std::filesystem::path& path, const Image& image, const int& quality = 100);

        static Image LoadImage(const std::filesystem::path& path);
        static Image DownloadImage(const std::string& url);

        static std::vector<unsigned char> EncodeImage(const Image& image);
    private:
        static FILE* GetFile(const std::filesystem::path& path);
        static void WriteToFile(void* ctx, void* data, int size);
};

#endif