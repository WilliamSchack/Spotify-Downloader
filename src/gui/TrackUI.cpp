#include "TrackUI.h"

TrackUI::TrackUI(const TrackData& data, QObject* parent) : QObject(parent), _data(data)
{
    FindCoverArtUrl();
}

int TrackUI::GetTrackNumber() const
{
    // TODO: Replace with function for TrackDownloader to use aswell
    int trackNumber = 1;
    switch (Config::TrackNumberType) {
        case ETrackNumberType::Playlist:
            trackNumber = _data.PlaylistTrackNumber == 0 ? _data.TrackNumber : _data.PlaylistTrackNumber;
            break;
        case ETrackNumberType::Album:
            trackNumber = _data.TrackNumber;
            break;
        case ETrackNumberType::Disc:
            trackNumber = _data.DiscNumber;
            break;
    }

    return trackNumber;
}

QString TrackUI::GetName() const
{
    return QString::fromStdString(_data.Name);
}

QString TrackUI::GetArtistNames() const
{
    return QString::fromStdString(MetadataManager::CombineArtistNames(_data.Artists, Config::ArtistSeperator));
}

QString TrackUI::GetAlbumName() const
{
    return QString::fromStdString(_data.Album.Name);
}

QString TrackUI::GetCoverArtUrl() const
{
    return _coverArtUrl;
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
    QString pathString = QString::fromStdString(FileUtils::PathToUtf8(path));
    _coverArtUrl = QUrl::fromLocalFile(pathString).toString();

    emit CoverArtUrlChanged();
}

void TrackUI::FindCoverArtUrl()
{
    std::filesystem::path path = TemporaryPaths::FindExistingTrackImagePath(_data);
    if (path.empty())
        return;

    SetCoverArtUrl(path);
}

void TrackUI::SetProgress(float progress)
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