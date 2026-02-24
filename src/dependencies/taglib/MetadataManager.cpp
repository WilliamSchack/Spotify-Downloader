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
            TagLib::ID3v2::TextIdentificationFrame* frame = new TagLib::ID3v2::TextIdentificationFrame("ICOP");
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
    TagLib::ByteVector taglibImage(reinterpret_cast<const char*>(image.Data), image.GetDataSize());
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

void MetadataManager::Close()
{
    // Bit hacky, but it assigns a new file ref so the old one is destroyed and the file can be used
    _fileRef = TagLib::FileRef();
}