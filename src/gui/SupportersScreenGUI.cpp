#include "SupportersScreenGUI.h"

SupportersScreenGUI::SupportersScreenGUI(QObject* parent) : QObject(parent)
{
    UpdateSupporters();
}

QList<SupporterSection> SupportersScreenGUI::GetSupporterSections()
{
    return _supporterSections;
}

void SupportersScreenGUI::UpdateSupporters()
{
    NetworkRequest request;
    request.Url = SUPPORTERS_URL;
    NetworkResponse response = request.Get();

    nlohmann::json json;
    if (response.CurlCode == CURLE_OK) {
        json = nlohmann::json::parse(response.Body);
        UpdateSupportersCache(json);
    } else {
        std::cout << "Failed to get notices, using cache: " << curl_easy_strerror(response.CurlCode) << std::endl;
        json = LoadSupportersCache();
        if (json.empty())
            return;
    }
    
    _supporterSections.clear();

    // Monthly supporters
    nlohmann::json currentSupportersJson = json["current_supporters"];
    for (auto it : currentSupportersJson.items()) {
        QList<Supporter> supporters;
        for (nlohmann::json supporterJson : it.value()) {
            QDate qDate = QDate::fromString(QString::fromStdString(supporterJson["start_date"]), "yyyy/MM/dd");
            int daysSince = qDate.daysTo(QDate::currentDate());

            QString timeString = "";
            if (daysSince < 31) {
                timeString = "For " + QString::number(daysSince) + " day" + (daysSince == 1 ? "" : "s");
            } else if (daysSince < 365) {
                // i know some months are 30 days icbf
                int monthsSince = std::floor(daysSince / 12.0);
                timeString = "For " + QString::number(monthsSince) + " month" + (monthsSince == 1 ? "" : "s");
            } else {
                int yearsSince = std::floor(daysSince / 365.0);
                timeString = "For " + QString::number(yearsSince) + " year" + (yearsSince == 1 ? "" : "s");
            }

            Supporter supporter;
            supporter.Name = QString::fromStdString(supporterJson["name"]);
            supporter.TimeString = timeString;
            
            supporters.push_back(supporter);
        }

        SupporterSection section;
        section.Title = QString::fromStdString(it.key()) + "s";
        section.Supporters = supporters;

        // Supporter tiers are in reverse order
        _supporterSections.push_front(section);
    }

    // One time supporters
    nlohmann::json oneTimeSupportersJson = json["one_time"];
    QList<Supporter> oneTimeSupporters;
    for (nlohmann::json supporterJson : oneTimeSupportersJson) {
        QDate qDate = QDate::fromString(QString::fromStdString(supporterJson["date"]), "yyyy/MM/dd");
        int daysSince = qDate.daysTo(QDate::currentDate());

        QString timeString = "";
        if (daysSince < 31) {
            timeString = QString::number(daysSince) + " day" + (daysSince == 1 ? "" : "s") + " ago";
        } else if (daysSince < 365) {
            // i know some months are 30 days icbf
            int monthsSince = std::floor(daysSince / 12.0);
            timeString = QString::number(monthsSince) + " month" + (monthsSince == 1 ? "" : "s") + " ago";
        } else {
            int yearsSince = std::floor(daysSince / 365.0);
            timeString = QString::number(yearsSince) + " year" + (yearsSince == 1 ? "" : "s") + " ago";
        }

        Supporter supporter;
        supporter.Name = QString::fromStdString(supporterJson["name"]);
        supporter.TimeString = timeString;

        oneTimeSupporters.push_back(supporter);
    }

    SupporterSection oneTimeSupportersSection;
    oneTimeSupportersSection.Title = "One Time Supporters";
    oneTimeSupportersSection.Supporters = oneTimeSupporters;
    _supporterSections.push_back(oneTimeSupportersSection);

    emit SupporterSectionsChanged();
}

void SupportersScreenGUI::UpdateSupportersCache(const nlohmann::json& json)
{
    std::filesystem::path filePath = GetSupportersCachePath();
    if (filePath.empty()) {
        std::cout << "Could not load notices cache, the file path was not found" << std::endl;
        return;
    }

    std::ofstream file(filePath);
    std::string contents = json.dump(4);
    file << contents;
    file.close();
}

nlohmann::json SupportersScreenGUI::LoadSupportersCache()
{
    std::filesystem::path filePath = GetSupportersCachePath();
    if (filePath.empty()) {
        std::cout << "Could not load notices cache, the file path was not found" << std::endl;
        return {};
    }

    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cout << "Could not open notices cache file at " << filePath << std::endl;
        return {};
    }

    std::string contents = std::string((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
    file.close();

    return nlohmann::json::parse(contents);
}

std::filesystem::path SupportersScreenGUI::GetSupportersCachePath()
{
    if (!_supportersFilePathCache.empty())
        return _supportersFilePathCache;

    std::filesystem::path configPath = Paths::GetConfigDir();
    if (configPath.empty())
        return "";

    _supportersFilePathCache = configPath / CACHE_FILE_NAME;
    return _supportersFilePathCache;
}