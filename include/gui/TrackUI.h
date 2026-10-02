#ifndef TRACKUI_H
#define TRACKUI_H

#include "TrackData.h"
#include "MetadataManager.h"
#include "MathUtils.h"
#include "TemporaryPaths.h"

#include <filesystem>

#include <QObject>
#include <QUrl>

class TrackUI : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int trackNumber READ GetTrackNumber CONSTANT)
    Q_PROPERTY(QString name READ GetName CONSTANT)
    Q_PROPERTY(QString artistNames READ GetArtistNames CONSTANT)
    Q_PROPERTY(QString albumName READ GetAlbumName CONSTANT)
    Q_PROPERTY(QString coverArtUrl READ GetCoverArtUrl NOTIFY CoverArtUrlChanged)
    Q_PROPERTY(float progress READ GetProgress NOTIFY ProgressChanged)
    Q_PROPERTY(QString status READ GetStatus NOTIFY StatusChanged)
    // TODO: Add Explicit

    public:
        explicit TrackUI(const TrackData& data, QObject* parent = nullptr);

        int GetTrackNumber() const;
        QString GetName() const;
        QString GetArtistNames() const;
        QString GetAlbumName() const;
        QString GetCoverArtUrl() const;

        float GetProgress() const;
        QString GetStatus() const;

        void SetCoverArtUrl(const std::filesystem::path& path);
        void FindCoverArtUrl();

        void SetProgress(float progress);
        void SetStatus(const std::string& status);
    private:
        TrackData _data;
        QString _coverArtUrl = "";

        float _progress = 0.0f;
        QString _status = "Queued For Download...";
    signals:
        void CoverArtUrlChanged();
        void ProgressChanged();
        void StatusChanged();
};

#endif