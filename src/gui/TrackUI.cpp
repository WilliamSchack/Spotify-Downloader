#include "TrackUI.h"

TrackUI::TrackUI(const TrackData& data, QObject* parent) : QObject(parent), _data(data)
{
    FindCoverArtUrl();
    AddSource(data.Platform, data.Url);
}

int TrackUI::GetTrackNumber() const
{
    return MetadataManager::GetTrackNumber(_data);
}

QString TrackUI::GetName() const
{
    return QString::fromStdString(_data.Name);
}

QString TrackUI::GetArtistNames() const
{
    return QString::fromStdString(MetadataManager::CombineArtistNames(_data.Artists, Config::ArtistSeparator));
}

QString TrackUI::GetAlbumName() const
{
    return QString::fromStdString(_data.Album.Name);
}

QString TrackUI::GetCoverArtUrl() const
{
    return _coverArtUrl;
}

QList<GUISourceInfo> TrackUI::GetSources() const
{
    return _sources;
}

float TrackUI::GetProgress() const
{
    return _progress;
}

QString TrackUI::GetStatus() const
{
    return _status;
}

void TrackUI::SetCoverArtUrl(const std::filesystem::path& path)
{
    // TODO:
    // Fix cover art with the same cover art, test downloading with an album (https://open.spotify.com/album/5l4pp1ptAaRFESQfrVMPdu?si=421e3706ef2e477f)
    // Sometimes it just doesnt work when multiple tracks use the same cover art, i have no clue

    QString pathString = QString::fromStdString(FileUtils::PathToUtf8(path));
    _coverArtUrl = QUrl::fromLocalFile(pathString).toString();

    emit CoverArtUrlChanged();
}

void TrackUI::FindCoverArtUrl()
{
    std::filesystem::path path = Paths::FindExistingTrackImagePath(_data);
    if (path.empty())
        return;

    SetCoverArtUrl(path);
}

void TrackUI::LoadCoverArt()
{
    std::thread([this]() {
        // Check if the cover art exists
        std::filesystem::path imageFilePath = Paths::GetTrackImagePathNoExtension(_data);
        std::filesystem::path imagePath = FileUtils::FindPathWithAnyExtension(imageFilePath.parent_path(), imageFilePath.filename());
        if (imagePath.empty()) {
            // Download the image
            Image image = ImageHandler::DownloadImage(_data.Album.ImageUrl);
            if (image.Data == nullptr)
                return;

            imagePath = ImageHandler::SaveImage(imageFilePath, image);
            if (imagePath.empty())
                return;
        }
        
        QMetaObject::invokeMethod(this, [this, imagePath]() {
            SetCoverArtUrl(imagePath);
        }, Qt::QueuedConnection);
    }).detach();
}

void TrackUI::AddSource(const EPlatform& platform, const std::string& link)
{
    std::cout << "ADDING SOURCE: " << (int)platform << std::endl;

    QString iconSource = "";
    QString qLink = QString::fromStdString(link);
    switch (platform) {
        case EPlatform::LocalFile:
            iconSource = "";
            qLink = QUrl::fromLocalFile(qLink).toString();
            break;
        case EPlatform::YouTube:
            iconSource = "qrc:/icons/platforms/YouTube.png";
            break;
        case EPlatform::Spotify:
            iconSource = "qrc:/icons/platforms/Spotify.png";
            break;
    }

    GUISourceInfo info;
    info.Platform = platform;
    info.IconSource = iconSource;
    info.Link = qLink;
    _sources.push_back(info);

    // Sort by platform index (I could insert based on the other values in the list but i cbf)
    std::sort(_sources.begin(), _sources.end(), [](const GUISourceInfo& s1, const GUISourceInfo& s2) {
        return (int)s1.Platform < (int)s2.Platform;
    });

    emit SourcesChanged();
}

void TrackUI::SetProgress(const float& progress)
{
    if (MathUtils::FloatsEqual(progress, _progress))
        return;

    _progress = progress;
    emit ProgressChanged();
}

void TrackUI::SetStatus(const std::string& status)
{
    QString qStatus = QString::fromStdString(status);
    if (qStatus == _status)
        return;

    _status = qStatus;
    emit StatusChanged();
}