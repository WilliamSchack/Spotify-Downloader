#ifndef MUSICSCREENGUI_H
#define MUSICSCREENGUI_H

#include "EasyQProperties.h"
#include "VariantUtils.h"
#include "DownloadManager.h"
#include "DownloadEvents.h"
#include "AddTracksPopup.h"
#include "TrackUI.h"
#include "Paths.h"

#include <iostream>
#include <map>
#include <thread>

#include <QObject>
#include <QList>

class MusicScreenGUI : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject* addTracksPopup READ GetAddTracksPopup CONSTANT)
    Q_PROPERTY(QList<QObject*> tracks READ GetTracks NOTIFY TracksChanged)
    Q_PROPERTY(QString playPauseButtonText READ GetPlayPauseButtonText NOTIFY PlayPauseButtonTextChanged);
    SIMPLE_QPROPERTY(QString, backgroundImagePath, BackgroundImagePath, _backgroundImagePath)
    SIMPLE_QPROPERTY(bool, loadingBackgroundImage, LoadingBackgroundImage, _loadingBackgroundImage)

    public:
        explicit MusicScreenGUI(QObject* parent = 0);

        AddTracksPopup* GetAddTracksPopup() const;
        QList<QObject*> GetTracks() const;

        QString GetPlayPauseButtonText() const;

        Q_INVOKABLE void UpdateBackgroundImage();
    public slots:
        void PlayPauseButtonPressed();
    private:
        void OnDownloadRequested(const std::string& link, const std::string& destinationFolder);
        void CheckDownloadEvents();

        void UpdateBackgroundImage(const PlaylistData& playlistData);
        void UpdateBackgroundImage(const std::string& url);

        void DisableBackgroundImageLoadingSafely();
    private:
        inline static const int DOWNLOAD_EVENTS_POLL_INTERVAL_MS = 100;

        AddTracksPopup* _addTracksPopup;

        DownloadManager _downloader;

        QList<QObject*> _tracks;
        std::map<std::string, TrackUI*> _tracksFromId;

        QString _backgroundImagePath;
        bool _loadingBackgroundImage = false;
    signals:
        void TracksChanged();
        void PlayPauseButtonTextChanged();
        void RequestNotification(const std::string& message);
};

#endif