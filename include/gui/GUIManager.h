#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include "MusicScreenGUI.h"
#include "SettingsScreenGUI.h"

#include <QObject>
#include <QQuickStyle>
#include <QSystemTrayIcon>
#include <QMenu>

class GUIManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject* music READ GetMusicManager CONSTANT)
    Q_PROPERTY(QObject* settings READ GetSettingsManager CONSTANT)

    public:
        explicit GUIManager(QObject* parent = 0);
    
        MusicScreenGUI* GetMusicManager() const;
        SettingsScreenGUI* GetSettingsManager() const;
    private:
        void SetupTrayIcon();
        void SendNotification(const std::string& message);
    private:
        static inline constexpr int NOTIFICATION_MS = 5000;

        MusicScreenGUI* _musicManager;
        SettingsScreenGUI* _settingsManager;

        QSystemTrayIcon _trayIcon;
};

#endif