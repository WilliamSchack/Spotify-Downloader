#ifndef IMAGE_H
#define IMAGE_H

struct Image
{
    int Width = 0;
    int Height = 0;
    int Channels;
    unsigned char* Data;
};

#endif