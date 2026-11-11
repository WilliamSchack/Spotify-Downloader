#ifndef SUPPORTERSSCREENGUI_H
#define SUPPORTERSSCREENGUI_H

#include "EasyQProperties.h"
#include "SupporterSection.h"
#include "NetworkRequest.h"
#include "Paths.h"

#include <thread>

#include <nlohmann/json.hpp>

#include <QObject>
#include <QList>
#include <QDate>

class SupportersScreenGUI : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<SupporterSection> supporterSections READ GetSupporterSections NOTIFY SupporterSectionsChanged)
    SIMPLE_QPROPERTY(bool, loadingSupporters, LoadingSupporters, _loadingSupporters)
    SIMPLE_QPROPERTY(bool, failedToGetSupporters, FailedToGetSupporters, _failedToGetSupporters)

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

        bool _loadingSupporters = false;
        bool _failedToGetSupporters = false;
    signals:
        void SupporterSectionsChanged();
};

#endif