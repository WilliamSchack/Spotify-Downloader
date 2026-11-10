#include "DependencyInfo.h"

DependencyInfo::DependencyInfo(std::function<bool()> installedFunction, std::function<bool()> installedOnSystemFunction, std::function<std::string()> getVersionFunction, std::function<bool()> updateAvailableFunction, std::function<bool()> updateFunction, std::function<bool()> downloadFunction, std::function<QList<GUIReleaseInfo>(int, int)> getReleasesFunction, QObject* parent)
    : QObject(parent), _installedFunction(installedFunction), _installedOnSystemFunction(installedOnSystemFunction), _getVersionFunction(getVersionFunction), _updateAvailableFunction(updateAvailableFunction), _updateFunction(updateFunction), _downloadFunction(downloadFunction), _getReleasesFunction(getReleasesFunction)
{
    CheckInstalled();
    UpdateVersion();
    CheckForUpdate();
    UpdateReleases(30, 1);
}

QList<GUIReleaseInfo> DependencyInfo::GetReleases()
{
    return _releases;
}

void DependencyInfo::UpdateReleases(const int& resultsPerPage, const int& page)
{
    std::thread([this, resultsPerPage, page]() {
        SetFailedToGetReleases(false);
        SetLoadingReleases(true);

        QList<GUIReleaseInfo> releases = _getReleasesFunction(resultsPerPage, page);

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