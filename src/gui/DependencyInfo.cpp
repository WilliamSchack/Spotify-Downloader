#include "DependencyInfo.h"

DependencyInfo::DependencyInfo(std::function<bool()> installedFunction, std::function<bool()> installedOnSystemFunction, std::function<std::string()> getVersionFunction, std::function<bool()> updateAvailableFunction, std::function<bool()> updateFunction, std::function<bool()> downloadFunction, std::function<QList<GUIReleaseInfo>(int, int)> getReleasesFunction, QObject* parent)
    : QObject(parent), _installedFunction(installedFunction), _installedOnSystemFunction(installedOnSystemFunction), _getVersionFunction(getVersionFunction), _updateAvailableFunction(updateAvailableFunction), _updateFunction(updateFunction), _downloadFunction(downloadFunction), _getReleasesFunction(getReleasesFunction)
{
    CheckInstalled();
    UpdateVersion();
    CheckForUpdate();
}

QList<GUIReleaseInfo> DependencyInfo::GetReleases()
{
    return _releases;
}

void DependencyInfo::UpdateReleases(const int& resultsPerPage, const int& page, const bool& useCache)
{
    // TODO: Add view more button

    std::thread([this, resultsPerPage, page, useCache]() {
        SetFailedToGetReleases(false);
        SetLoadingReleases(true);

        QList<GUIReleaseInfo> releases;
        if (useCache) {
            releases = LoadReleasesCache();
            
            if (releases.size() == 0)
                releases = _getReleasesFunction(resultsPerPage, page);
        } else {
            releases = _getReleasesFunction(resultsPerPage, page);
        }

        if (releases.size() != 0)
            UpdateReleasesCache(releases);

        // Size down headers, they get too big in the gui otherwise
        for (GUIReleaseInfo& release : releases) {
            std::string body = release.Body.toStdString();
            body = StringUtils::ReplaceAll(release.Body.toStdString(), "# ", "## ");
            body = StringUtils::ReplaceAll(release.Body.toStdString(), "## ", "### ");

            release.Body = QString::fromStdString(body);
        }

        // Call gui stuff on the main thread
        QMetaObject::invokeMethod(this, [this, releases]() {
            SetLoadingReleases(false);

            if (releases.size() == 0) {
                SetFailedToGetReleases(true);
            } else {
                _releases = releases;
                emit ReleasesChanged();
            }
        }, Qt::QueuedConnection);
    }).detach();
}

void DependencyInfo::UpdateReleasesCache(const QList<GUIReleaseInfo>& releases)
{
    std::filesystem::path filePath = GetReleasesCachePath();
    if (filePath.empty()) {
        std::cout << "Could not load releases cache for" << _name.toStdString() << ", the file path was not found" << std::endl;
        return;
    }

    nlohmann::json json = nlohmann::json::array();
    for (GUIReleaseInfo release : releases) {
        nlohmann::json releaseJson = {
            { "name", release.Name.toStdString() },
            { "body", release.Body.toStdString() }
        };

        json.push_back(releaseJson);
    };

    std::ofstream file(filePath);
    std::string contents = json.dump(4);
    file << contents;
    file.close();
}

QList<GUIReleaseInfo> DependencyInfo::LoadReleasesCache()
{
    std::filesystem::path filePath = GetReleasesCachePath();
    if (filePath.empty()) {
        std::cout << "Could not load releases cache for" << _name.toStdString() << ", the file path was not found" << std::endl;
        return {};
    }

    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cout << "Could not open releases cache file at " << filePath << std::endl;
        return {};
    }

    std::string contents = std::string((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
    file.close();

    // Parse file
    nlohmann::json json = nlohmann::json::parse(contents);
    if (!json.is_array()) {
        std::cout << "Could not read releases json for " << _name.toStdString() << std::endl;
        return {};
    }

    QList<GUIReleaseInfo> releases;
    for (nlohmann::json releaseJson : json) {
        GUIReleaseInfo release;
        release.Name = QString::fromStdString(releaseJson["name"]);
        release.Body = QString::fromStdString(releaseJson["body"]);

        releases.push_back(release);
    }

    return releases;
}

std::filesystem::path DependencyInfo::GetReleasesCachePath()
{
    if (!_releasesFilePathCache.empty())
        return _releasesFilePathCache;

    std::filesystem::path configPath = Paths::GetConfigDir();
    if (configPath.empty())
        return "";

    _releasesFilePathCache = configPath / (_name.toStdString() + "_Releases.json");
    return _releasesFilePathCache;
}

void DependencyInfo::CheckInstalled()
{
    SetInstalled(_installedFunction());
    SetInstalledOnSystem(_installedOnSystemFunction());
}

void DependencyInfo::CheckForUpdate()
{
    CheckInstalled();
    if (!_installed || _installedOnSystem)
        return;

    std::thread([this]() {
        SetCheckingForUpdates(true);

        bool updateAvailable = _updateAvailableFunction();

        // Call gui stuff on the main thread
        QMetaObject::invokeMethod(this, [this, updateAvailable]() {
            SetUpdateAvailable(updateAvailable);
            SetCheckingForUpdates(false);

            if (_updateAvailable)
                UpdateReleases(30, 1, false);
            else
                UpdateReleases();
        }, Qt::QueuedConnection);
    }).detach();
}   

void DependencyInfo::DownloadUpdate()
{
    CheckInstalled();
    if (!_installed || _installedOnSystem)
        return;

    std::thread([this]() {
        SetUpdating(true);

        bool updated = _updateFunction();

        // Call gui stuff on the main thread
        QMetaObject::invokeMethod(this, [this, updated]() {
            // TODO: Popup when update failed

            SetUpdating(false);

            if (updated) {
                std::cout << "Successfully updated " << _name.toStdString() << std::endl;
                SetUpdateAvailable(false);
                UpdateVersion();
            } else {
                std::cout << "Failed to update " << _name.toStdString() << std::endl;
                SetUpdateAvailable(true);
            }
        }, Qt::QueuedConnection);
    }).detach();
}

void DependencyInfo::Download()
{
    CheckInstalled();
    if (_installed)
        return;

    std::thread([this]() {
        SetDownloading(true);

        bool downloaded = _downloadFunction();

        // Call gui stuff on the main thread
        QMetaObject::invokeMethod(this, [this, downloaded]() {
            // TODO: Popup when download failed

            SetDownloading(false);

            if (downloaded) {
                std::cout << "Successfully downloaded " << _name.toStdString() << std::endl;
                CheckInstalled();
                UpdateVersion();
                SetUpdateAvailable(false);
            } else {
                std::cout << "Failed to download " << _name.toStdString() << std::endl;
            }
        }, Qt::QueuedConnection);
    }).detach();
}

void DependencyInfo::UpdateVersion()
{
    SetVersion(QString::fromStdString(_getVersionFunction()));
}