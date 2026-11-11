#include "NoticesScreenGUI.h"

NoticesScreenGUI::NoticesScreenGUI(QObject* parent) : QObject(parent)
{
    UpdateNotices();
}

Q_INVOKABLE void NoticesScreenGUI::SelectNotice(const int& index)
{
    _selectedNoticeIndex = index;
    emit SelectedNoticeChanged();
}

QList<Notice> NoticesScreenGUI::GetNotices()
{
    return _notices;
}

Notice NoticesScreenGUI::GetSelectedNotice()
{
    if (_notices.size() <= _selectedNoticeIndex)
        return {};

    return _notices[_selectedNoticeIndex];
}

void NoticesScreenGUI::UpdateNotices()
{
    SetFailedToGetNotices(false);
    SetLoadingNotices(true);

    std::thread([this]() {
        NetworkRequest request;
        request.Url = NOTICES_URL;
        NetworkResponse response = request.Get();

        nlohmann::json json;
        if (response.CurlCode == CURLE_OK) {
            json = nlohmann::json::parse(response.Body);
            UpdateNoticesCache(json);
        } else {
            std::cout << "Failed to get notices, using cache: " << curl_easy_strerror(response.CurlCode) << std::endl;
            json = LoadNoticesCache();
            if (json.empty())
                return;
        }

        QList<Notice> notices;
        for (int i = 0; i < json.size(); i++) {
            nlohmann::json noticeJson = json[i];

            Notice notice;
            notice.Index = i;
            notice.ReleaseDate = QString::fromStdString(noticeJson["release_date"]);
            notice.Title = QString::fromStdString(noticeJson["title"]);
            notice.Content = QString::fromStdString(noticeJson["content"]);

            notices.push_back(notice);
        }

        QMetaObject::invokeMethod(this, [this, notices]() {
            SetLoadingNotices(false);

            if (notices.size() == 0) {
                SetFailedToGetNotices(true);
            } else {
                _notices = notices;
                SelectNotice(_selectedNoticeIndex);
                emit NoticesChanged();
            }
        }, Qt::QueuedConnection);
    }).detach();

    emit NoticesChanged();
}

void NoticesScreenGUI::UpdateNoticesCache(const nlohmann::json& json)
{
    std::filesystem::path filePath = GetNoticesCachePath();
    if (filePath.empty()) {
        std::cout << "Could not load notices cache, the file path was not found" << std::endl;
        return;
    }

    std::ofstream file(filePath);
    std::string contents = json.dump(4);
    file << contents;
    file.close();
}

nlohmann::json NoticesScreenGUI::LoadNoticesCache()
{
    std::filesystem::path filePath = GetNoticesCachePath();
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

    nlohmann::json json = nlohmann::json::parse(contents);
    if (!json.is_array()) {
        std::cout << "Could not read notices json" << std::endl;
        return {};
    }

    return json;
}

std::filesystem::path NoticesScreenGUI::GetNoticesCachePath()
{
    if (!_noticesFilePathCache.empty())
        return _noticesFilePathCache;

    std::filesystem::path cachePath = Paths::GetCacheDir();
    if (cachePath.empty())
        return "";

    _noticesFilePathCache = cachePath / CACHE_FILE_NAME;
    return _noticesFilePathCache;
}