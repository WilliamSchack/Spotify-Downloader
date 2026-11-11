#ifndef SUPPORTERSSCREENGUI_H
#define SUPPORTERSSCREENGUI_H

#include "SupporterSection.h"
#include "NetworkRequest.h"
#include "Paths.h"

#include <nlohmann/json.hpp>

#include <QObject>
#include <QList>
#include <QDate>

class SupportersScreenGUI : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<SupporterSection> supporterSections READ GetSupporterSections NOTIFY SupporterSectionsChanged)

    public:
        explicit SupportersScreenGUI(QObject* parent = 0);

        Q_INVOKABLE void UpdateSupporters();

        QList<SupporterSection> GetSupporterSections();
    private:
        void UpdateSupportersCache(const nlohmann::json& json);
        nlohmann::json LoadSupportersCache();
        std::filesystem::path GetSupportersCachePath();
    private:
        static inline const std::string SUPPORTERS_URL = "https://api.echolite.app/supporters";
        static inline const std::string CACHE_FILE_NAME = "Supporters.json";

        QList<SupporterSection> _supporterSections;

        std::filesystem::path _supportersFilePathCache = "";
    signals:
        void SupporterSectionsChanged();
};

#endif