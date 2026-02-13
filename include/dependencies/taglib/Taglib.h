#ifndef TAGLIBWRAPPER_H
#define TAGLIBWRAPPER_H
// TAGLIB_H is used in taglib

#include "Config.h"
#include "CodecFactory.h"
#include "TrackData.h"
#include "Image.h"

#include <taglib/fileref.h>
#include <taglib/tstringlist.h>

#include <taglib/id3v2tag.h>
#include <taglib/id3v2frame.h>
#include <taglib/commentsframe.h>
#include <taglib/attachedpictureframe.h>
#include <taglib/textidentificationframe.h>
#include <taglib/unsynchronizedlyricsframe.h>
#include <taglib/synchronizedlyricsframe.h>

#include <taglib/mp4file.h>
#include <taglib/mp4tag.h>
#include <taglib/mp4coverart.h>

#include <taglib/wavfile.h>

#include <taglib/flacpicture.h>
#include <taglib/xiphcomment.h>

#include <filesystem>

class Taglib
{
    public:
        Taglib(const std::filesystem::path& filePath);

        static void SetTitle        (const std::string& value);
        static void SetArtists      (const std::string& value);
        static void SetAlbumName    (const std::string& value);
        static void SetAlbumArtists (const std::string& value);
        static void SetPublisher    (const std::string& value);
        static void SetCopyright    (const std::string& value);
        static void SetComment      (const std::string& value);
        static void SetReleaseDate  (const std::string& value);
        static void SetTrackNumber  (const std::string& value);
        static void SetDiscNumber   (const std::string& value);
        static void SetLyrics       (const std::string& value);
        static void SetCoverImage   (const Image& image);
    private:
        TagLib::FileRef _fileRef;
        std::unique_ptr<ICodec> _codec;
};

#endif