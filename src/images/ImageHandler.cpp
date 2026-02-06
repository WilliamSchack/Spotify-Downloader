#include "ImageHandler.h"

bool ImageHandler::SavePng(const std::filesystem::path& path, const Image& image)
{
    return stbi_write_png(
        path.c_str(),
        image.Width,
        image.Height,
        image.Channels,
        image.Data,
        image.Width * image.Channels
    );
};

bool ImageHandler::SaveJpg(const std::filesystem::path& path, const Image& image, const int& quality)
{
    return stbi_write_jpg(
        path.c_str(),
        image.Width,
        image.Height,
        image.Channels,
        image.Data,
        quality
    );
};

Image ImageHandler::LoadImage(const std::filesystem::path& path)
{
    Image image;
    image.Data = stbi_load(
        path.c_str(),
        &image.Width,
        &image.Height,
        &image.Channels,
        0
    );

    return image;
};