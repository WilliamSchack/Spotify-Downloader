#include "XSPFFile.h"

std::string XSPFFile::GetExtension()
{
    return "xspf";
}

std::string XSPFFile::WriteHeader()
{
    return
        R"(<?xml version="1.0" encoding="UTF-8"?>)" "\n"
        R"(<playlist xmlns="http://xspf.org/ns/0/" version="1">)" "\n"
        "\t<trackList>\n";
}

std::string XSPFFile::WriteFile(const PlaylistFileTrack& track)
{
    MetadataManager metadata(track.Path);

    // Start tags
    std::string output = "\t\t<track>\n";

    // Details
    output += "\t\t\t<location>" + track.EncodedPath + "</location>\n";
    output += "\t\t\t<title>" + track.Data.Name + "</title>\n";
    output += "\t\t\t<creator>" + track.Data.GetMainArtist().Name + "</creator>\n";
    output += "\t\t\t<album>" + track.Data.Album.Name + "</album>\n";
    output += "\t\t\t<annotation>" + metadata.GetComment() + "</annotation>\n";
    if (track.Data.TrackNumber != 0)
        output += "\t\t\t<trackNum>" + std::to_string(track.Data.TrackNumber) + "</trackNum>\n";
    output += "\t\t\t<duration>" + std::to_string(track.Data.DurationMilliseconds) + "</duration>\n";

    // End tags
    output += "\t\t</track>\n";

    return output;
}

std::string XSPFFile::WriteFooter()
{
    return
        "\t</trackList>\n"
        "</playlist>";
}
