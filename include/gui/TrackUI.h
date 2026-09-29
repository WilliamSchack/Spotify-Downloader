#ifndef TRACKUI_H
#define TRACKUI_H

#include "TrackData.h"
#include "MetadataManager.h"
#include "MathUtils.h"

#include <QObject>

class TrackUI : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int trackNumber READ GetTrackNumber CONSTANT)
    Q_PROPERTY(QString name READ GetName CONSTANT)
    Q_PROPERTY(QString artistNames READ GetArtistNames CONSTANT)
    Q_PROPERTY(QString albumName READ GetAlbumName CONSTANT)
    Q_PROPERTY(float progress READ GetProgress NOTIFY ProgressChanged)
    Q_PROPERTY(QString status READ GetStatus NOTIFY StatusChanged)

    public:
        explicit TrackUI(const TrackData& data, QObject* parent = nullptr);

        int GetTrackNumber() const;
        QString GetName() const;
        QString GetArtistNames() const;
        QString GetAlbumName() const;
        
        float GetProgress() const;
        QString GetStatus() const;

        void SetProgress(float progress);
        void SetStatus(const std::string& status);
    signals:
        void ProgressChanged();
        void StatusChanged();
    private:
        TrackData _data;

        float _progress = 0.0f;
        QString _status = "";
};

#endif