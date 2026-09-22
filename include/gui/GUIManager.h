#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include "MusicScreenGUI.h"

#include <QObject>

class GUIManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject* music READ GetMusicManager CONSTANT)

    public:
        explicit GUIManager(QObject* parent = 0);
    
        MusicScreenGUI* GetMusicManager() const;
    private:
        MusicScreenGUI* _musicManager;
};

#endif