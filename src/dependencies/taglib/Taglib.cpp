#include "Taglib.h"

Taglib::Taglib(const std::filesystem::path& filePath)
{
    if (filePath.empty() || !std::filesystem::exists(filePath))
        return;

    _fileRef = TagLib::FileRef(filePath.string().c_str(), true, TagLib::AudioProperties::Accurate);
    _codec = CodecFactory::Create(filePath.extension());
}

void Taglib::SetTitle(const std::string& value) {}
void Taglib::SetArtists(const std::string& value) {}
void Taglib::SetAlbumName(const std::string& value) {}
void Taglib::SetAlbumArtists(const std::string& value) {}
void Taglib::SetPublisher(const std::string& value) {}
void Taglib::SetCopyright(const std::string& value) {}
void Taglib::SetComment(const std::string& value) {}
void Taglib::SetReleaseDate(const std::string& value) {}
void Taglib::SetTrackNumber(const std::string& value) {}
void Taglib::SetDiscNumber(const std::string& value) {}
void Taglib::SetLyrics(const std::string& value) {}
void Taglib::SetCoverImage(const Image& image) {}