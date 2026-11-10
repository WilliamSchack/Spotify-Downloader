#ifndef NOTICESSCREENGUI_H
#define NOTICESSCREENGUI_H

#include "Notice.h"
#include "NetworkRequest.h"
#include "Paths.h"
#include "StringUtils.h"

#include <string>

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

    public:
        explicit NoticesScreenGUI(QObject* parent = 0);

        Q_INVOKABLE void SelectNotice(const int& index);

        QList<Notice> GetNotices();
        Notice GetSelectedNotice();
    private:
        void UpdateNotices();
        
        void UpdateNoticesCache(const nlohmann::json& json);
        nlohmann::json LoadNoticesCache();
        std::filesystem::path GetNoticesCachePath();
    private:
        static inline const std::string NOTICES_URL = "https://echolite.app/notices.json";
        static inline const std::string CACHE_FILE_NAME = "Notices.json";

        QList<Notice> _notices;
        int _selectedNoticeIndex = 0;

        std::filesystem::path _noticesFilePathCache = "";
    signals:
        void NoticesChanged();
        void SelectedNoticeChanged();
};

#endif