#ifndef METADATAMANAGER_H
#define METADATAMANAGER_H

#include "Config.h"
#include "CodecFactory.h"
#include "TrackData.h"
#include "ArtistData.h"
#include "ImageHandler.h"

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

class MetadataManager
{
    public:
        MetadataManager(const std::filesystem::path& filePath);

        void SetTitle       (const std::string& value);
        void SetArtist      (const std::string& value);
        void SetAlbumName   (const std::string& value);
        void SetAlbumArtist (const std::string& value);
        void SetPublisher   (const std::string& value);
        void SetCopyright   (const std::string& value);
        void SetComment     (const std::string& value);
        void SetReleaseDate (const std::string& value);
        void SetTrackNumber (const unsigned int& value);
        void SetDiscNumber  (const unsigned int& value);
        void SetLyrics      (const std::string& value);
        void SetCoverImage  (const Image& image);

        void SetArtists(const std::vector<ArtistData>& artists);
        void SetAlbumArtists(const std::vector<ArtistData>& artists);

        void Close();
    private:
        std::string CombineArtistNames(const std::vector<ArtistData>& artists);
    private:
        TagLib::FileRef _fileRef;
        std::unique_ptr<ICodec> _codec;
};

#endif