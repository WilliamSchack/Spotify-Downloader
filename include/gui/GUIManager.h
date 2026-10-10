#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include "MusicScreenGUI.h"
#include "SettingsScreenGUI.h"

#include <QObject>
#include <QQuickStyle>

class GUIManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject* music READ GetMusicManager CONSTANT)
    Q_PROPERTY(QObject* settings READ GetSettingsManager CONSTANT)

    public:
        explicit GUIManager(QWindow* mainWindow, QObject* parent = 0);
    
        MusicScreenGUI* GetMusicManager() const;
        SettingsScreenGUI* GetSettingsManager() const;
    private:
        QWindow* _mainWindow;
        MusicScreenGUI* _musicManager;
        SettingsScreenGUI* _settingsManager;
};

#endif