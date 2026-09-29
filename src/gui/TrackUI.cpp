#include "TrackUI.h"

TrackUI::TrackUI(const TrackData& data, QObject* parent) : QObject(parent), _data(data)
{
    std::filesystem::path coverArtPath = TemporaryPaths::FindExistingTrackImagePath(data);
    if (coverArtPath.empty())
        return;

    QString coverArtPathString = QString::fromStdString(FileUtils::PathToUtf8(coverArtPath));
    _coverArtUrl = QUrl::fromLocalFile(coverArtPathString).toString();
}

int TrackUI::GetTrackNumber() const
{
    return _data.TrackNumber;
}

QString TrackUI::GetName() const
{
    return QString::fromStdString(_data.Name);
}

QString TrackUI::GetArtistNames() const
{
    return QString::fromStdString(MetadataManager::CombineArtistNames(_data.Artists));
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