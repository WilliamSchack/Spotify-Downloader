#include "ImageHandler.h"

std::string ImageHandler::GetImageFormatString(const EImageFormat& format)
{
    switch (format) {
        case EImageFormat::PNG: return "png";
        case EImageFormat::JPG: return "jpg";
    }

    return "";
}

EImageFormat ImageHandler::GetImageFormat(const std::string& string)
{
    std::string format = string;
    StringUtils::ToLower(format);

    if (format == "png")                     return EImageFormat::PNG;
    if (format == "jpg" || format == "jpeg") return EImageFormat::JPG;

    return EImageFormat::Unknown;
}

EImageFormat ImageHandler::GetImageFormat(const std::filesystem::path& path)
{
    std::string pathString = path.string();
    unsigned int startIndex = pathString.find_last_of('.') + 1;
    std::string formatString = pathString.substr(startIndex);
    return GetImageFormat(formatString);
}

EImageFormat ImageHandler::GetImageFormat(const NetworkResponse& response)
{
    if (response.Body.size() < 4)
    return EImageFormat::Unknown;
    
    // Check bytes
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(response.Body.data());

    if (bytes[0] == 0x89 && bytes[1] == 0x50 && bytes[2] == 0x4E && bytes[3] == 0x47)
        return EImageFormat::PNG;

    if (bytes[0] == 0xFF && bytes[1] == 0xD8 && bytes[2] == 0xFF)
        return EImageFormat::JPG;

    return EImageFormat::Unknown;
}

bool ImageHandler::SaveImage(const std::filesystem::path& pathNoExtension, const Image& image)
{
    std::filesystem::path path = pathNoExtension.wstring() + L"." + StringUtils::ToWString(GetImageFormatString(image.Format));
    if (image.Format == EImageFormat::PNG) return SavePng(path, image);
    if (image.Format == EImageFormat::JPG) return SaveJpg(path, image);

    std::cout << "Unable to save image, format is not implemented" << std::endl;

    return false;
}

bool ImageHandler::SavePng(const std::filesystem::path& path, const Image& image)
{
    FILE* file = GetFile(path);
    if (!file) return false;

    int result = stbi_write_png_to_func(
        WriteToFile,
        file,
        image.Width,
        image.Height,
        image.Channels,
        image.Data,
        image.Width * image.Channels
    );

    fclose(file);
    return result != 0;
};

bool ImageHandler::SaveJpg(const std::filesystem::path& path, const Image& image, const int& quality)
{
    FILE* file = GetFile(path);
    if (!file) return false;

    int result = stbi_write_jpg_to_func(
        WriteToFile,
        file,
        image.Width,
        image.Height,
        image.Channels,
        image.Data,
        quality
    );

    fclose(file);
    return result != 0;
};

Image ImageHandler::LoadImage(const std::filesystem::path& path)
{
    Image image;

    FILE* file = GetFile(path);
    if (!file) return image;

    image.Format = GetImageFormat(path);
    image.Data = stbi_load_from_file(
        file,
        &image.Width,
        &image.Height,
        &image.Channels,
        0
    );

    fclose(file);
    return image;
};

Image ImageHandler::DownloadImage(const std::string& url)
{
    Image image;

    NetworkRequest request;
    request.Url = url;
    NetworkResponse response = request.Get();
    if (response.Body.empty()) return image;

    image.Format = GetImageFormat(response);
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

std::vector<unsigned char> ImageHandler::EncodeImage(const Image& image)
{
    if (image.Format == EImageFormat::Unknown)
        return std::vector<unsigned char>();

    std::vector<unsigned char> buffer;
    
    auto callback = [](void* context, void* data, int size) {
        std::vector<unsigned char>* buffer = reinterpret_cast<std::vector<unsigned char>*>(context);
        unsigned char* bytes = reinterpret_cast<unsigned char*>(data);
        buffer->insert(buffer->end(), bytes, bytes + size);
    };
    
    switch (image.Format) {
        case EImageFormat::PNG:
            stbi_write_png_to_func(
                callback,
                &buffer,
                image.Width,
                image.Height,
                image.Channels,
                image.Data,
                image.Width * image.Channels
            );
            break;
        case EImageFormat::JPG:
            stbi_write_jpg_to_func(
                callback,
                &buffer,
                image.Width,
                image.Height,
                image.Channels,
                image.Data,
                100
            );
            break;
    }

    return buffer;
}

FILE* ImageHandler::GetFile(const std::filesystem::path& path)
{
#if WIN32
    // On windows the wstring is required for some file names to work correctly
    FILE* file = _wfopen(path.wstring().c_str(), L"wb");
#else
    FILE* file = fopen(path.c_str(), "wb");
#endif

    return file;
}

void ImageHandler::WriteToFile(void* ctx, void* data, int size)
{
    fwrite(data, 1, size, static_cast<FILE*>(ctx));
}