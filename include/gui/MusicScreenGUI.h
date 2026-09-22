#ifndef MUSICSCREENGUI_H
#define MUSICSCREENGUI_H

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
    private:
        AddTracksPopup* _addTracksPopup;
};

#endif