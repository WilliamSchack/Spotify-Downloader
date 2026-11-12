#ifndef TRACKUI_H
#define TRACKUI_H

#include "TrackData.h"
#include "GUISourceInfo.h"
#include "MetadataManager.h"
#include "MathUtils.h"
#include "Paths.h"
#include "ImageHandler.h"

#include <algorithm>
#include <filesystem>
#include <thread>

#include <QObject>
#include <QList>
#include <QUrl>

class TrackUI : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int trackNumber READ GetTrackNumber CONSTANT)
    Q_PROPERTY(QString name READ GetName CONSTANT)
    Q_PROPERTY(QString artistNames READ GetArtistNames CONSTANT)
    Q_PROPERTY(QString albumName READ GetAlbumName CONSTANT)
    Q_PROPERTY(QString coverArtUrl READ GetCoverArtUrl NOTIFY CoverArtUrlChanged)
    Q_PROPERTY(QList<GUISourceInfo> sources READ GetSources NOTIFY SourcesChanged)
    Q_PROPERTY(float progress READ GetProgress NOTIFY ProgressChanged)
    Q_PROPERTY(QString status READ GetStatus NOTIFY StatusChanged)
    // TODO: Add Explicit Field

    public:
        explicit TrackUI(const TrackData& data, QObject* parent = nullptr);

        int GetTrackNumber() const;
        QString GetName() const;
        QString GetArtistNames() const;
        QString GetAlbumName() const;
        QString GetCoverArtUrl() const;
        QList<GUISourceInfo> GetSources() const;

        float GetProgress() const;
        QString GetStatus() const;

        void SetCoverArtUrl(const std::filesystem::path& path);
        void FindCoverArtUrl();
        Q_INVOKABLE void LoadCoverArt();

        void AddSource(const EPlatform& platform, const std::string& link);

        void SetProgress(const float& progress);
        void SetStatus(const std::string& status);
    private:
        TrackData _data;
        QString _coverArtUrl = "";
        QList<GUISourceInfo> _sources;

        float _progress = 0.0f;
        QString _status = "Queued For Download...";
    signals:
        void CoverArtUrlChanged();
        void SourcesChanged();
        void ProgressChanged();
        void StatusChanged();
};

#endif