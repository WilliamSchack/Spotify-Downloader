#include "NoticesScreenGUI.h"

NoticesScreenGUI::NoticesScreenGUI(QObject* parent) : QObject(parent)
{
    if (Config::AutoCheckForNotices)
        UpdateNotices();
}

Q_INVOKABLE void NoticesScreenGUI::SelectNotice(const int& index)
{
    _selectedNoticeIndex = index;
    emit SelectedNoticeChanged();
}

QList<Notice*> NoticesScreenGUI::GetNotices()
{
    return _notices;
}

Notice* NoticesScreenGUI::GetSelectedNotice()
{
    if (_notices.size() <= _selectedNoticeIndex)
        return nullptr;

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

        nlohmann::json noticesCache = LoadNoticesCache();

        nlohmann::json json;
        if (response.CurlCode == CURLE_OK) {
            json = nlohmann::json::parse(response.Body);
            UpdateNoticesCache(json);
        } else {
            Logger::Warning("Failed to get notices, using cache: " + std::string(curl_easy_strerror(response.CurlCode)));
            json = noticesCache;
            if (noticesCache.empty())
                return;
        }
        QMetaObject::invokeMethod(this, [this, json, noticesCache]() {
            // Must make the notices on the main thread

            // Mark any new notices
            // Dont mark new notices if this is the first time opening the app
            int noticesDifference = 0;
            if (noticesCache.size() != 0)
                noticesDifference = json.size() - noticesCache.size();

            _newNotices = noticesDifference;
            SetNewNoticeAvailable(noticesDifference > 0);

            QList<Notice*> notices;
            for (int i = 0; i < json.size(); i++) {
                nlohmann::json noticeJson = json[i];

                Notice* notice = new Notice(this);
                notice->Index = i;
                notice->ReleaseDate = QString::fromStdString(noticeJson["release_date"]);
                notice->Title = QString::fromStdString(noticeJson["title"]);
                notice->Content = QString::fromStdString(noticeJson["content"]);
                notice->SetIsNew(noticesDifference > 0);

                connect(notice, &Notice::IsNewChanged, this, [this]() {
                    _newNotices--;
                    if (_newNotices <= 0)
                        SetNewNoticeAvailable(false);
                });

                notices.push_back(notice);

                if (noticesDifference > 0)
                    noticesDifference--;
            }

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
        Logger::Warning("Could not load notices cache, the file path was not found");
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
        Logger::Warning("Could not load notices cache, the file path was not found");
        return {};
    }

    std::ifstream file(filePath);
    if (!file.is_open()) {
        Logger::Warning("Could not open notices cache file at " + FileUtils::PathToUtf8(filePath));
        return {};
    }

    std::string contents = std::string((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
    file.close();

    nlohmann::json json = nlohmann::json::parse(contents);
    if (!json.is_array()) {
        Logger::Warning("Could not read notices json");
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