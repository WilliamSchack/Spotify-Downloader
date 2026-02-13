#include "MetadataManager.h"

MetadataManager::MetadataManager(const std::filesystem::path& filePath)
{
    if (filePath.empty() || !std::filesystem::exists(filePath))
        return;

    _fileRef = TagLib::FileRef(filePath.string().c_str(), true, TagLib::AudioProperties::Accurate);
    _codec = CodecFactory::Create(filePath.extension());
}

void MetadataManager::SetTitle(const std::string& value) {}
void MetadataManager::SetArtists(const std::string& value) {}
void MetadataManager::SetAlbumName(const std::string& value) {}
void MetadataManager::SetAlbumArtists(const std::string& value) {}
void MetadataManager::SetPublisher(const std::string& value) {}
void MetadataManager::SetCopyright(const std::string& value) {}
void MetadataManager::SetComment(const std::string& value) {}
void MetadataManager::SetReleaseDate(const std::string& value) {}
void MetadataManager::SetTrackNumber(const std::string& value) {}
void MetadataManager::SetDiscNumber(const std::string& value) {}
void MetadataManager::SetLyrics(const std::string& value) {}
void MetadataManager::SetCoverImage(const Image& image) {}