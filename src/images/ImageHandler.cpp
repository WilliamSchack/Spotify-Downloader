#include "ImageHandler.h"

bool ImageHandler::SaveImage(const std::filesystem::path& path, const Image& image)
{
    std::string pathString = path.string();
    unsigned int startIndex = pathString.find_last_of('.') + 1;
    std::string format = pathString.substr(startIndex);
    StringUtils::ToLower(format);

    if (format == "png")                     return SavePng(path, image);
    if (format == "jpg" || format == "jpeg") return SaveJpg(path, image);

    std::cout << "Unable to save image, format (" << format << ") is not implemented" << std::endl;

    return false;
}

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

Image ImageHandler::DownloadImage(const std::string& url)
{
    Image image;

    NetworkRequest request;
    request.Url = url;
    NetworkResponse response = request.Get();
    if (response.Body.empty()) return image;

    image.Data = stbi_load_from_memory(
        reinterpret_cast<const stbi_uc*>(response.Body.data()),
        response.Body.size(),
        &image.Width,
        &image.Height,
        &image.Channels,
        0
    );

    return image;
}