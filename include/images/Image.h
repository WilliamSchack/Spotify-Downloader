#ifndef IMAGE_H
#define IMAGE_H

#include "stb_image.h"

struct Image
{
    int Width = 0;
    int Height = 0;
    int Channels = 0;
    unsigned char* Data;

    Image() : Data{} {}

    ~Image() {
        if (Data)
            stbi_image_free(Data);
    }

    // Move constructor
    Image(Image&& other)
        : Width(other.Width), Height(other.Height), Channels(other.Channels), Data(other.Data)
    {
        other.Data = nullptr;
    }

    // Non-copyable, causes double free if copied
    Image (const Image&) = delete;
    Image& operator= (const Image&) = delete;

};

#endif