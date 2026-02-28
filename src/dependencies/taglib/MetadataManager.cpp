#include "MetadataManager.h"

MetadataManager::MetadataManager(const std::filesystem::path& filePath)
{
    if (filePath.empty() || !std::filesystem::exists(filePath))
        return;

    _fileRef = TagLib::FileRef(filePath.string().c_str(), true, TagLib::AudioProperties::Accurate);
    _codec = CodecFactory::Create(filePath.extension());
}

void MetadataManager::SetTitle(const std::string& value)
{
    TagLib::String taglibString(value.c_str(), TagLib::String::UTF8);
    TagLib::Tag* fileTag = _codec->GetFileTag(_fileRef);

    switch (_codec->GetMetadataType()) {
        case EMetadataType::ID3V2:
            dynamic_cast<TagLib::ID3v2::Tag*>(fileTag)->setTitle(taglibString);
            break;
        case EMetadataType::MP4:
            dynamic_cast<TagLib::MP4::Tag*>(fileTag)->setTitle(taglibString);
            break;
        case EMetadataType::XIPH:
            dynamic_cast<TagLib::Ogg::XiphComment*>(fileTag)->setTitle(taglibString);
            break;
        case EMetadataType::RIFF:
            dynamic_cast<TagLib::RIFF::Info::Tag*>(fileTag)->setTitle(taglibString);
            break;
    }
}

void MetadataManager::SetArtist(const std::string& value)
{
    TagLib::String taglibString(value.c_str(), TagLib::String::UTF8);
    TagLib::Tag* fileTag = _codec->GetFileTag(_fileRef);

    switch (_codec->GetMetadataType()) {
        case EMetadataType::ID3V2:
            dynamic_cast<TagLib::ID3v2::Tag*>(fileTag)->setArtist(taglibString);
            break;
        case EMetadataType::MP4:
            dynamic_cast<TagLib::MP4::Tag*>(fileTag)->setArtist(taglibString);
            break;
        case EMetadataType::XIPH:
            dynamic_cast<TagLib::Ogg::XiphComment*>(fileTag)->setArtist(taglibString);
            break;
        case EMetadataType::RIFF:
            dynamic_cast<TagLib::RIFF::Info::Tag*>(fileTag)->setArtist(taglibString);
            break;
    }
}

void MetadataManager::SetArtists(const std::vector<ArtistData>& artists)
{
    SetArtist(CombineArtistNames(artists));
};

void MetadataManager::SetAlbumName(const std::string& value)
{
    TagLib::String taglibString(value.c_str(), TagLib::String::UTF8);
    TagLib::Tag* fileTag = _codec->GetFileTag(_fileRef);

    switch (_codec->GetMetadataType()) {
        case EMetadataType::ID3V2:
            dynamic_cast<TagLib::ID3v2::Tag*>(fileTag)->setAlbum(taglibString);
            break;
        case EMetadataType::MP4:
            dynamic_cast<TagLib::MP4::Tag*>(fileTag)->setAlbum(taglibString);
            break;
        case EMetadataType::XIPH:
            dynamic_cast<TagLib::Ogg::XiphComment*>(fileTag)->setAlbum(taglibString);
            break;
        case EMetadataType::RIFF:
            dynamic_cast<TagLib::RIFF::Info::Tag*>(fileTag)->setAlbum(taglibString);
            break;
    }
}

void MetadataManager::SetAlbumArtist(const std::string& value)
{
    TagLib::String taglibString(value.c_str(), TagLib::String::UTF8);
    TagLib::Tag* fileTag = _codec->GetFileTag(_fileRef);

    switch (_codec->GetMetadataType()) {
        case EMetadataType::ID3V2: {
            TagLib::ID3v2::TextIdentificationFrame* frame = new TagLib::ID3v2::TextIdentificationFrame("TPE2");
            frame->setText(taglibString);

            dynamic_cast<TagLib::ID3v2::Tag*>(fileTag)->addFrame(frame);
            break;
        } case EMetadataType::MP4: {
            dynamic_cast<TagLib::MP4::Tag*>(fileTag)->setItem("aART", TagLib::StringList(taglibString));
            break;
        } case EMetadataType::XIPH: {
            dynamic_cast<TagLib::Ogg::XiphComment*>(fileTag)->addField("ALBUMARTIST", taglibString);
            break;
        }
    }
}

void MetadataManager::SetAlbumArtists(const std::vector<ArtistData>& artists)
{
    SetAlbumArtist(CombineArtistNames(artists));
};

void MetadataManager::SetPublisher(const std::string& value)
{
    TagLib::String taglibString(value.c_str(), TagLib::String::UTF8);
    TagLib::Tag* fileTag = _codec->GetFileTag(_fileRef);

    switch (_codec->GetMetadataType()) {
        case EMetadataType::ID3V2: {
            TagLib::ID3v2::TextIdentificationFrame* frame = new TagLib::ID3v2::TextIdentificationFrame("TPUB");
            frame->setText(taglibString);

            dynamic_cast<TagLib::ID3v2::Tag*>(fileTag)->addFrame(frame);
            break;
        } case EMetadataType::MP4: {
            // test me
            dynamic_cast<TagLib::MP4::Tag*>(fileTag)->setItem("labl", TagLib::StringList(taglibString));
            break;
        } case EMetadataType::XIPH: {
            dynamic_cast<TagLib::Ogg::XiphComment*>(fileTag)->addField("PUBLISHER", taglibString);
            break;
        }
    }
}

void MetadataManager::SetCopyright(const std::string& value)
{
    TagLib::String taglibString(value.c_str(), TagLib::String::UTF8);
    TagLib::Tag* fileTag = _codec->GetFileTag(_fileRef);

    switch (_codec->GetMetadataType()) {
        case EMetadataType::ID3V2: {
            TagLib::ID3v2::TextIdentificationFrame* frame = new TagLib::ID3v2::TextIdentificationFrame("TCOP");
            frame->setText(taglibString);

            dynamic_cast<TagLib::ID3v2::Tag*>(fileTag)->addFrame(frame);
            break;
        } case EMetadataType::MP4: {
            // test me
            dynamic_cast<TagLib::MP4::Tag*>(fileTag)->setItem("cprt", TagLib::StringList(taglibString));
            break;
        } case EMetadataType::XIPH: {
            dynamic_cast<TagLib::Ogg::XiphComment*>(fileTag)->addField("COPYRIGHT", taglibString);
            break;
        }
    }
}

void MetadataManager::SetComment(const std::string& value)
{
    TagLib::String taglibString(value.c_str(), TagLib::String::UTF8);
    TagLib::Tag* fileTag = _codec->GetFileTag(_fileRef);

    switch (_codec->GetMetadataType()) {
        case EMetadataType::ID3V2: {
            TagLib::ID3v2::CommentsFrame* frame = new TagLib::ID3v2::CommentsFrame();
            frame->setText(taglibString);

            dynamic_cast<TagLib::ID3v2::Tag*>(fileTag)->addFrame(frame);
            break;
        } case EMetadataType::MP4: {
            dynamic_cast<TagLib::MP4::Tag*>(fileTag)->setComment(taglibString);
            break;
        } case EMetadataType::XIPH: {
            dynamic_cast<TagLib::Ogg::XiphComment*>(fileTag)->setComment(taglibString);
            break;
        } case EMetadataType::RIFF: {
            dynamic_cast<TagLib::RIFF::Info::Tag*>(fileTag)->setComment(taglibString);
            break;
        }
    }
}

void MetadataManager::SetReleaseDate(const std::string& value)
{
    TagLib::String taglibString(value.c_str(), TagLib::String::UTF8);
    TagLib::Tag* fileTag = _codec->GetFileTag(_fileRef);

    switch (_codec->GetMetadataType()) {
        case EMetadataType::ID3V2: {
            TagLib::ID3v2::TextIdentificationFrame* frame = new TagLib::ID3v2::TextIdentificationFrame("TDRC");
            frame->setText(taglibString);

            dynamic_cast<TagLib::ID3v2::Tag*>(fileTag)->addFrame(frame);
            break;
        } case EMetadataType::MP4: {
            dynamic_cast<TagLib::MP4::Tag*>(fileTag)->setItem("\251day", TagLib::StringList(taglibString));
            break;
        } case EMetadataType::XIPH: {
            dynamic_cast<TagLib::Ogg::XiphComment*>(fileTag)->addField("DATE", taglibString);
            break;
        } case EMetadataType::RIFF: {
            dynamic_cast<TagLib::RIFF::Info::Tag*>(fileTag)->setFieldText("ICRD", taglibString);
            break;
        }
    }
}

void MetadataManager::SetTrackNumber(const unsigned int& value)
{
    TagLib::Tag* fileTag = _codec->GetFileTag(_fileRef);

    switch (_codec->GetMetadataType()) {
        case EMetadataType::ID3V2: {
            TagLib::ID3v2::TextIdentificationFrame* frame = new TagLib::ID3v2::TextIdentificationFrame("TRCK");
            frame->setText(std::to_string(value));

            dynamic_cast<TagLib::ID3v2::Tag*>(fileTag)->addFrame(frame);
            break;
        } case EMetadataType::MP4: {
            dynamic_cast<TagLib::MP4::Tag*>(fileTag)->setTrack(value);
            break;
        } case EMetadataType::XIPH: {
            dynamic_cast<TagLib::Ogg::XiphComment*>(fileTag)->setTrack(value);
            break;
        } case EMetadataType::RIFF: {
            dynamic_cast<TagLib::RIFF::Info::Tag*>(fileTag)->setTrack(value);
            break;
        }
    }
}

void MetadataManager::SetDiscNumber(const unsigned int& value)
{
    std::string valueString = std::to_string(value);
    TagLib::Tag* fileTag = _codec->GetFileTag(_fileRef);

    switch (_codec->GetMetadataType()) {
        case EMetadataType::ID3V2: {
            TagLib::ID3v2::TextIdentificationFrame* frame = new TagLib::ID3v2::TextIdentificationFrame("TPOS");
            frame->setText(valueString);

            dynamic_cast<TagLib::ID3v2::Tag*>(fileTag)->addFrame(frame);
            break;
        } case EMetadataType::MP4: {
            dynamic_cast<TagLib::MP4::Tag*>(fileTag)->setItem("disk", TagLib::StringList(valueString));
            break;
        } case EMetadataType::XIPH: {
            dynamic_cast<TagLib::Ogg::XiphComment*>(fileTag)->addField("DISCNUMBER", valueString);
            break;
        }
    }
}

void MetadataManager::SetLyrics(const std::string& value)
{
    TagLib::String taglibString(value.c_str(), TagLib::String::UTF8);
    TagLib::Tag* fileTag = _codec->GetFileTag(_fileRef);

    switch (_codec->GetMetadataType()) {
        case EMetadataType::ID3V2: {
            TagLib::ID3v2::UnsynchronizedLyricsFrame* frame = new TagLib::ID3v2::UnsynchronizedLyricsFrame();
            frame->setText(taglibString);

            dynamic_cast<TagLib::ID3v2::Tag*>(fileTag)->addFrame(frame);
            break;
        } case EMetadataType::MP4: {
            dynamic_cast<TagLib::MP4::Tag*>(fileTag)->setItem("\251lyr", TagLib::StringList(taglibString));
            break;
        } case EMetadataType::XIPH: {
            dynamic_cast<TagLib::Ogg::XiphComment*>(fileTag)->addField("LYRICS", taglibString);
            break;
        }
    }
}

void MetadataManager::SetCoverImage(const Image& image)
{
    std::vector<unsigned char> imageEncodedBytes = ImageHandler::EncodeImage(image);
    TagLib::ByteVector taglibImage(reinterpret_cast<const char*>(imageEncodedBytes.data()), imageEncodedBytes.size());
    TagLib::Tag* fileTag = _codec->GetFileTag(_fileRef);

    // Cover art override
    if (_codec->SetCoverArt(_fileRef, taglibImage))
        return;

    switch (_codec->GetMetadataType()) {
        case EMetadataType::ID3V2: {
            TagLib::ID3v2::AttachedPictureFrame* picFrame = new TagLib::ID3v2::AttachedPictureFrame();
            picFrame->setPicture(taglibImage);
            picFrame->setMimeType("image/png");
            picFrame->setType(TagLib::ID3v2::AttachedPictureFrame::FrontCover);

            dynamic_cast<TagLib::ID3v2::Tag*>(fileTag)->addFrame(picFrame);
            break;
        } case EMetadataType::MP4: {
            TagLib::MP4::CoverArt coverArt(TagLib::MP4::CoverArt::Format::PNG, taglibImage);
            TagLib::MP4::CoverArtList coverArtList;
            coverArtList.append(coverArt);
            TagLib::MP4::Item coverItem(coverArtList);

            dynamic_cast<TagLib::MP4::Tag*>(fileTag)->setItem("covr", coverItem);
            break;
        } case EMetadataType::XIPH: {
            TagLib::FLAC::Picture* coverArt = new TagLib::FLAC::Picture();
				coverArt->setData(taglibImage);
				coverArt->setMimeType("image/png");
				coverArt->setType(TagLib::FLAC::Picture::Type::FrontCover);

            dynamic_cast<TagLib::Ogg::XiphComment*>(fileTag)->addPicture(coverArt);
            break;
        }
    }
}

std::string MetadataManager::GetTitle() const
{
    return GetStringField(EMetadataTag::Title);
}

std::string MetadataManager::GetArtist() const
{
    return GetStringField(EMetadataTag::Artist);
}

std::string MetadataManager::GetAlbumName() const
{
    return GetStringField(EMetadataTag::AlbumName);
}

std::string MetadataManager::GetAlbumArtist() const
{
    return GetStringField(EMetadataTag::AlbumArtist);
}

std::string MetadataManager::GetPublisher() const
{
    return GetStringField(EMetadataTag::Publisher);
}

std::string MetadataManager::GetCopyright() const
{
    return GetStringField(EMetadataTag::Copyright);
}

std::string MetadataManager::GetComment() const
{
    return GetStringField(EMetadataTag::Comment);
}

std::string MetadataManager::GetReleaseDate() const
{
    return GetStringField(EMetadataTag::ReleaseDate);
}

unsigned int MetadataManager::GetTrackNumber() const
{
    std::string fieldString = GetStringField(EMetadataTag::TrackNumber);
    if (fieldString.empty()) return 0;

    return std::stoi(fieldString);
}

unsigned int MetadataManager::GetDiscNumber() const
{
    std::string fieldString = GetStringField(EMetadataTag::DiscNumber);
    if (fieldString.empty()) return 0;

    return std::stoi(fieldString);
}

const char* MetadataManager::GetTagId(const EMetadataTag& tag) const
{
    // Tags that are handled through taglib functions are not listed here

    EMetadataType metadataType = _codec->GetMetadataType();
    if (metadataType == EMetadataType::None) return "";

    if (metadataType == EMetadataType::ID3V2) {
        switch (tag) {
            case EMetadataTag::Title:       return "TIT2";
            case EMetadataTag::Artist:      return "TPE1";
            case EMetadataTag::AlbumName:   return "TALB";
            case EMetadataTag::AlbumArtist: return "TPE2";
            case EMetadataTag::Publisher:   return "TPUB";
            case EMetadataTag::Copyright:   return "TCOP";
            case EMetadataTag::Comment:     return "COMM";
            case EMetadataTag::ReleaseDate: return "TDRC";
            case EMetadataTag::TrackNumber: return "TRCK";
            case EMetadataTag::DiscNumber:  return "TPOS";
        };

        return "";
    }

    if (metadataType == EMetadataType::MP4) {
        switch (tag) {
            case EMetadataTag::Title:       return "\251nam";
            case EMetadataTag::Artist:      return "\251art";
            case EMetadataTag::AlbumName:   return "\251alb";
            case EMetadataTag::AlbumArtist: return "aART";
            case EMetadataTag::Publisher:   return "\251pub";
            case EMetadataTag::Copyright:   return "cprt";
            case EMetadataTag::Comment:     return "\251cmt";
            case EMetadataTag::ReleaseDate: return "\251day";
            case EMetadataTag::TrackNumber: return "trkn";
            case EMetadataTag::DiscNumber:  return "disk";
            case EMetadataTag::Lyrics:      return "\251lyr";
            case EMetadataTag::CoverImage:  return "covr";
        };

        return "";
    }

    if (metadataType == EMetadataType::XIPH) {
        switch (tag) {
            case EMetadataTag::Title:       return "TITLE";
            case EMetadataTag::Artist:      return "ARTIST";
            case EMetadataTag::AlbumName:   return "ALBUM";
            case EMetadataTag::AlbumArtist: return "ALBUMARTIST";
            case EMetadataTag::Publisher:   return "PUBLISHER";
            case EMetadataTag::Copyright:   return "COPYRIGHT";
            case EMetadataTag::Comment:     return "COMMENT";
            case EMetadataTag::ReleaseDate: return "DATE";
            case EMetadataTag::TrackNumber: return "TRACKNUMBER";
            case EMetadataTag::DiscNumber:  return "DISCNUMBER";
            case EMetadataTag::Lyrics:      return "LYRICS";
        };

        return "";
    }

    if (metadataType == EMetadataType::RIFF) {
        switch (tag) {
            case EMetadataTag::Title:       return "INAM";
            case EMetadataTag::Artist:      return "IART";
            case EMetadataTag::AlbumName:   return "IPRD";
            case EMetadataTag::Copyright:   return "ICOP";
            case EMetadataTag::Comment:     return "ICMT";
            case EMetadataTag::ReleaseDate: return "ICRD";
            case EMetadataTag::TrackNumber: return "IPRT";
        };

        return "";
    }

    return "";
}

std::string MetadataManager::GetStringField(const EMetadataTag& tag) const
{
    TagLib::Tag* fileTag = _codec->GetFileTag(_fileRef);
    const char* tagId = GetTagId(tag);
    if (tagId == "") return "";

    switch (_codec->GetMetadataType()) {
        case EMetadataType::ID3V2: {
            const TagLib::ID3v2::FrameList& frames = dynamic_cast<TagLib::ID3v2::Tag*>(fileTag)->frameList(tagId);
            if (frames.size() == 0) return "";

            return frames[0]->toString().to8Bit(true);
        } case EMetadataType::MP4: {
            TagLib::MP4::Item item = dynamic_cast<TagLib::MP4::Tag*>(fileTag)->item(tagId);
            if (!item.isValid()) return "";

            return item.toStringList().toString().to8Bit(true);
        } case EMetadataType::XIPH: {
            const TagLib::Ogg::FieldListMap& fields = dynamic_cast<TagLib::Ogg::XiphComment*>(fileTag)->fieldListMap();
            if (!fields.contains(tagId)) return "";

            return fields[tagId][0].to8Bit(true);
        } case EMetadataType::RIFF: {
            const TagLib::RIFF::Info::FieldListMap& fields = dynamic_cast<TagLib::RIFF::Info::Tag*>(fileTag)->fieldListMap();
            if (!fields.contains(tagId)) return "";

            return fields[tagId].to8Bit(true);
        }
    }
    
    return "";
}

std::string MetadataManager::CombineArtistNames(const std::vector<ArtistData>& artists) const
{
    std::string connected = "";

    unsigned int artistsSize = artists.size();
    for (int i = 0; i < artistsSize; i++) {
        connected += artists[i].Name;

        if (i < artistsSize - 1)
            connected += Config::ARTISTS_SEPERATOR;
    }

    return connected;
}

void MetadataManager::Close()
{
    _fileRef.save();

    // Bit hacky, but it assigns a new file ref so the old one is destroyed and the file can be used
    _fileRef = TagLib::FileRef();
}