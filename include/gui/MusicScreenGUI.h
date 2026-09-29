#ifndef MUSICSCREENGUI_H
#define MUSICSCREENGUI_H

#include "VariantUtils.h"
#include "DownloadManager.h"
#include "DownloadEvents.h"
#include "AddTracksPopup.h"
#include "TrackUI.h"

#include <iostream>
#include <map>

#include <QObject>
#include <QList>

class MusicScreenGUI : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject* addTracksPopup READ GetAddTracksPopup CONSTANT)
    Q_PROPERTY(QList<QObject*> tracks READ GetTracks NOTIFY TracksChanged)

    public:
        explicit MusicScreenGUI(QObject* parent = 0);

        AddTracksPopup* GetAddTracksPopup() const;
        QList<QObject*> GetTracks() const;
    private:
        void OnDownloadRequested(const std::string& link, const std::string& destinationFolder);
        void CheckDownloadEvents();
    private:
        inline static const int DOWNLOAD_EVENTS_POLL_INTERVAL_MS = 100;

        AddTracksPopup* _addTracksPopup;

        DownloadManager _downloader;

        QList<QObject*> _tracks;
        std::map<std::string, TrackUI*> _tracksFromId;
    signals:
        void TracksChanged();
};

#endif