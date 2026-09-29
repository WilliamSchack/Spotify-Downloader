#ifndef MUSICSCREENGUI_H
#define MUSICSCREENGUI_H

#include "VariantUtils.h"
#include "DownloadManager.h"
#include "DownloadEvents.h"
#include "AddTracksPopup.h"

#include <iostream>

#include <QObject>

class MusicScreenGUI : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject* addTracksPopup READ GetAddTracksPopup CONSTANT)

    public:
        explicit MusicScreenGUI(QObject* parent = 0);

        AddTracksPopup* GetAddTracksPopup() const;
    private:
        void OnDownloadRequested(const std::string& link, const std::string& destinationFolder);
        void CheckDownloadEvents();
    private:
        inline static const int DOWNLOAD_EVENTS_POLL_INTERVAL_MS = 100;

        AddTracksPopup* _addTracksPopup;
        
        DownloadManager _downloader;

};

#endif