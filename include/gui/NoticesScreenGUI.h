#ifndef NOTICESSCREENGUI_H
#define NOTICESSCREENGUI_H

#include "EasyQProperties.h"
#include "Notice.h"
#include "NetworkRequest.h"
#include "Paths.h"

#include <string>
#include <thread>

#include <nlohmann/json.hpp>

#include <QObject>
#include <QList>
#include <QtQml/qqml.h>

// TODO: Add alert when new notice

class NoticesScreenGUI : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<Notice> notices READ GetNotices NOTIFY NoticesChanged)
    Q_PROPERTY(Notice selectedNotice READ GetSelectedNotice NOTIFY SelectedNoticeChanged)
    SIMPLE_QPROPERTY(bool, loadingNotices, LoadingNotices, _loadingNotices)
    SIMPLE_QPROPERTY(bool, failedToGetNotices, FailedToGetNotices, _failedToGetNotices)
    
    public:
        explicit NoticesScreenGUI(QObject* parent = 0);

        Q_INVOKABLE void SelectNotice(const int& index);
        Q_INVOKABLE void UpdateNotices();

        QList<Notice> GetNotices();
        Notice GetSelectedNotice();
    private:        
        void UpdateNoticesCache(const nlohmann::json& json);
        nlohmann::json LoadNoticesCache();
        std::filesystem::path GetNoticesCachePath();
    private:
        static inline const std::string NOTICES_URL = "https://api.echolite.app/notices";
        static inline const std::string CACHE_FILE_NAME = "Notices.json";

        QList<Notice> _notices;
        int _selectedNoticeIndex = 0;

        std::filesystem::path _noticesFilePathCache = "";

        bool _loadingNotices = false;
        bool _failedToGetNotices = false;
    signals:
        void NoticesChanged();
        void SelectedNoticeChanged();
};

#endif