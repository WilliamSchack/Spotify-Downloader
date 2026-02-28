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

    _fileRef.save();
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

    _fileRef.save();
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

    _fileRef.save();
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

    _fileRef.save();
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

    _fileRef.save();
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

    _fileRef.save();
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

    _fileRef.save();
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

    _fileRef.save();
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

    _fileRef.save();
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
            dynamic_cast<TagLib::MP4::Tag*>(fileTag)->setItem("disc", TagLib::StringList(valueString));
            break;
        } case EMetadataType::XIPH: {
            dynamic_cast<TagLib::Ogg::XiphComment*>(fileTag)->addField("DISCNUMBER", valueString);
            break;
        }
    }

    _fileRef.save();
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

    _fileRef.save();
}

void MetadataManager::SetCoverImage(const Image& image)
{
    std::vector<unsigned char> imageEncodedBytes = ImageHandler::EncodeImage(image);
    TagLib::ByteVector taglibImage(reinterpret_cast<const char*>(imageEncodedBytes.data()), imageEncodedBytes.size());
    TagLib::Tag* fileTag = _codec->GetFileTag(_fileRef);

    // Cover art override
    if (_codec->SetCoverArt(_fileRef, taglibImage)) {
        _fileRef.save();
        return;
    }

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

    _fileRef.save();
}

std::string MetadataManager::GetTitle() const
{
    TagLib::Tag* fileTag = _codec->GetFileTag(_fileRef);

    switch (_codec->GetMetadataType()) {
        case EMetadataType::ID3V2:
            return dynamic_cast<TagLib::ID3v2::Tag*>(fileTag)->title().to8Bit(true);
        case EMetadataType::MP4:
            return dynamic_cast<TagLib::MP4::Tag*>(fileTag)->title().to8Bit(true);
        case EMetadataType::XIPH:
            return dynamic_cast<TagLib::Ogg::XiphComment*>(fileTag)->title().to8Bit(true);
        case EMetadataType::RIFF:
            return dynamic_cast<TagLib::RIFF::Info::Tag*>(fileTag)->title().to8Bit(true);
    }

    return "";
}

std::string MetadataManager::GetArtist() const
{
    TagLib::Tag* fileTag = _codec->GetFileTag(_fileRef);

    switch (_codec->GetMetadataType()) {
        case EMetadataType::ID3V2:
            return dynamic_cast<TagLib::ID3v2::Tag*>(fileTag)->artist().to8Bit(true);
        case EMetadataType::MP4:
            return dynamic_cast<TagLib::MP4::Tag*>(fileTag)->artist().to8Bit(true);
        case EMetadataType::XIPH:
            return dynamic_cast<TagLib::Ogg::XiphComment*>(fileTag)->artist().to8Bit(true);
        case EMetadataType::RIFF:
            return dynamic_cast<TagLib::RIFF::Info::Tag*>(fileTag)->artist().to8Bit(true);
    }

    return "";
}

std::string MetadataManager::GetAlbumName() const
{
    TagLib::Tag* fileTag = _codec->GetFileTag(_fileRef);

    switch (_codec->GetMetadataType()) {
        case EMetadataType::ID3V2:
            return dynamic_cast<TagLib::ID3v2::Tag*>(fileTag)->album().to8Bit(true);
        case EMetadataType::MP4:
            return dynamic_cast<TagLib::MP4::Tag*>(fileTag)->album().to8Bit(true);
        case EMetadataType::XIPH:
            return dynamic_cast<TagLib::Ogg::XiphComment*>(fileTag)->album().to8Bit(true);
        case EMetadataType::RIFF:
            return dynamic_cast<TagLib::RIFF::Info::Tag*>(fileTag)->album().to8Bit(true);
    }

    return "";
}

std::string MetadataManager::GetAlbumArtist() const
{
    TagLib::Tag* fileTag = _codec->GetFileTag(_fileRef);

    switch (_codec->GetMetadataType()) {
        case EMetadataType::ID3V2: {
            const TagLib::ID3v2::FrameList& frames = dynamic_cast<TagLib::ID3v2::Tag*>(fileTag)->frameList("TPE2");
            if (frames.size() == 0) return "";

            return frames[0]->toString().to8Bit(true);
        } case EMetadataType::MP4: {
            TagLib::MP4::Item item = dynamic_cast<TagLib::MP4::Tag*>(fileTag)->item("aART");
            if (!item.isValid()) return "";

            return item.toStringList().toString().to8Bit(true);
        } case EMetadataType::XIPH: {
            const TagLib::Ogg::FieldListMap& fields = dynamic_cast<TagLib::Ogg::XiphComment*>(fileTag)->fieldListMap();
            if (!fields.contains("ALBUMARTIST")) return "";

            return fields["ALBUMARTIST"][0].to8Bit(true);
        }
    }

    return "";
}

std::string MetadataManager::GetPublisher() const
{
    TagLib::Tag* fileTag = _codec->GetFileTag(_fileRef);

    switch (_codec->GetMetadataType()) {
        case EMetadataType::ID3V2: {
            const TagLib::ID3v2::FrameList& frames = dynamic_cast<TagLib::ID3v2::Tag*>(fileTag)->frameList("TPUB");
            if (frames.size() == 0) return "";

            return frames[0]->toString().to8Bit(true);
        } case EMetadataType::MP4: {
            TagLib::MP4::Item item = dynamic_cast<TagLib::MP4::Tag*>(fileTag)->item("labl");
            if (!item.isValid()) return "";

            return item.toStringList().toString().to8Bit(true);
        } case EMetadataType::XIPH: {
            const TagLib::Ogg::FieldListMap& fields = dynamic_cast<TagLib::Ogg::XiphComment*>(fileTag)->fieldListMap();
            if (!fields.contains("PUBLISHER")) return "";

            return fields["PUBLISHER"][0].to8Bit(true);
        }
    }

    return "";
}

std::string MetadataManager::GetCopyright() const
{
    TagLib::Tag* fileTag = _codec->GetFileTag(_fileRef);

    switch (_codec->GetMetadataType()) {
        case EMetadataType::ID3V2: {
            const TagLib::ID3v2::FrameList& frames = dynamic_cast<TagLib::ID3v2::Tag*>(fileTag)->frameList("TCOP");
            if (frames.size() == 0) return "";

            return frames[0]->toString().to8Bit(true);
        } case EMetadataType::MP4: {
            TagLib::MP4::Item item = dynamic_cast<TagLib::MP4::Tag*>(fileTag)->item("cprt");
            if (!item.isValid()) return "";

            return item.toStringList().toString().to8Bit(true);
        } case EMetadataType::XIPH: {
            const TagLib::Ogg::FieldListMap& fields = dynamic_cast<TagLib::Ogg::XiphComment*>(fileTag)->fieldListMap();
            if (!fields.contains("COPYRIGHT")) return "";

            return fields["COPYRIGHT"][0].to8Bit(true);
        }
    }

    return "";
}

std::string MetadataManager::GetComment() const
{
    TagLib::Tag* fileTag = _codec->GetFileTag(_fileRef);

    switch (_codec->GetMetadataType()) {
        case EMetadataType::ID3V2: {
            const TagLib::ID3v2::FrameList& frames = dynamic_cast<TagLib::ID3v2::Tag*>(fileTag)->frameList("COMM");
            if (frames.size() == 0) return "";

            return frames[0]->toString().to8Bit(true);
        } case EMetadataType::MP4: {
            return dynamic_cast<TagLib::MP4::Tag*>(fileTag)->comment().to8Bit(true);
        } case EMetadataType::XIPH: {
            return dynamic_cast<TagLib::Ogg::XiphComment*>(fileTag)->comment().to8Bit(true);
        } case EMetadataType::RIFF: {
            return dynamic_cast<TagLib::RIFF::Info::Tag*>(fileTag)->comment().to8Bit(true);
        }
    }

    return "";
}

std::string MetadataManager::GetReleaseDate() const
{
    TagLib::Tag* fileTag = _codec->GetFileTag(_fileRef);

    switch (_codec->GetMetadataType()) {
        case EMetadataType::ID3V2: {
            const TagLib::ID3v2::FrameList& frames = dynamic_cast<TagLib::ID3v2::Tag*>(fileTag)->frameList("TDRC");
            if (frames.size() == 0) return "";

            return frames[0]->toString().to8Bit(true);
        } case EMetadataType::MP4: {
            TagLib::MP4::Item item = dynamic_cast<TagLib::MP4::Tag*>(fileTag)->item("\251day");
            if (!item.isValid()) return "";

            return item.toStringList().toString().to8Bit(true);
        } case EMetadataType::XIPH: {
            const TagLib::Ogg::FieldListMap& fields = dynamic_cast<TagLib::Ogg::XiphComment*>(fileTag)->fieldListMap();
            if (!fields.contains("DATE")) return "";

            return fields["DATE"][0].to8Bit(true);
        } case EMetadataType::RIFF: {
            return dynamic_cast<TagLib::RIFF::Info::Tag*>(fileTag)->fieldText("ICRD").to8Bit();
        }
    }
    
    return "";
}

unsigned int MetadataManager::GetTrackNumber() const
{
    TagLib::Tag* fileTag = _codec->GetFileTag(_fileRef);

    switch (_codec->GetMetadataType()) {
        case EMetadataType::ID3V2: {
            const TagLib::ID3v2::FrameList& frames = dynamic_cast<TagLib::ID3v2::Tag*>(fileTag)->frameList("TRCK");
            if (frames.size() == 0) return 0;

            return std::stoi(frames[0]->toString().to8Bit(true));
        } case EMetadataType::MP4: {
            dynamic_cast<TagLib::MP4::Tag*>(fileTag)->track();
            break;
        } case EMetadataType::XIPH: {
            dynamic_cast<TagLib::Ogg::XiphComment*>(fileTag)->track();
            break;
        } case EMetadataType::RIFF: {
            dynamic_cast<TagLib::RIFF::Info::Tag*>(fileTag)->track();
            break;
        }
    }

    return 0;
}

unsigned int MetadataManager::GetDiscNumber() const
{
    TagLib::Tag* fileTag = _codec->GetFileTag(_fileRef);

    switch (_codec->GetMetadataType()) {
        case EMetadataType::ID3V2: {
            const TagLib::ID3v2::FrameList& frames = dynamic_cast<TagLib::ID3v2::Tag*>(fileTag)->frameList("TPOS");
            if (frames.size() == 0) return 0;

            return std::stoi(frames[0]->toString().to8Bit(true));
        } case EMetadataType::MP4: {
            TagLib::MP4::Item item = dynamic_cast<TagLib::MP4::Tag*>(fileTag)->item("disc");
            if (!item.isValid()) return 0;

            return std::stoi(item.toStringList().toString().to8Bit(true));
        } case EMetadataType::XIPH: {
            const TagLib::Ogg::FieldListMap& fields = dynamic_cast<TagLib::Ogg::XiphComment*>(fileTag)->fieldListMap();
            if (!fields.contains("DISCNUMBER")) return 0;

            return std::stoi(fields["DISCNUMBER"][0].to8Bit(true));
        }
    }

    return 0;
}

void MetadataManager::Close()
{
    // Bit hacky, but it assigns a new file ref so the old one is destroyed and the file can be used
    _fileRef = TagLib::FileRef();
}

std::string MetadataManager::CombineArtistNames(const std::vector<ArtistData>& artists)
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