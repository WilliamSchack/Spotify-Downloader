#include "Taglib.h"

void Taglib::SetTitle(const std::filesystem::path& filePath, const std::string& value) {}
void Taglib::SetArtists(const std::filesystem::path& filePath, const std::string& value) {}
void Taglib::SetAlbumName(const std::filesystem::path& filePath, const std::string& value) {}
void Taglib::SetAlbumArtists(const std::filesystem::path& filePath, const std::string& value) {}
void Taglib::SetPublisher(const std::filesystem::path& filePath, const std::string& value) {}
void Taglib::SetCopyright(const std::filesystem::path& filePath, const std::string& value) {}
void Taglib::SetComment(const std::filesystem::path& filePath, const std::string& value) {}
void Taglib::SetReleaseDate(const std::filesystem::path& filePath, const std::string& value) {}
void Taglib::SetTrackNumber(const std::filesystem::path& filePath, const std::string& value) {}
void Taglib::SetDiscNumber(const std::filesystem::path& filePath, const std::string& value) {}
void Taglib::SetLyrics(const std::filesystem::path& filePath, const std::string& value) {}
void Taglib::SetCoverImage(const std::filesystem::path& filePath, const Image& image) {}

TagLib::FileRef Taglib::GetFileRef(const std::filesystem::path& filePath)
{
    return TagLib::FileRef(filePath.string().c_str(), true, TagLib::AudioProperties::Accurate);
}