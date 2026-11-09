#include "DependencyInfo.h"

DependencyInfo::DependencyInfo(std::function<std::string()> getVersionFunction, std::function<bool()> updateAvailableFunction, std::function<bool()> updateFunction, QObject* parent)
    : QObject(parent), _getVersionFunction(getVersionFunction), _updateAvailableFunction(updateAvailableFunction), _updateFunction(updateFunction)
{
    UpdateVersion();
    CheckForUpdate();
}

#include <iostream>

void DependencyInfo::CheckForUpdate()
{
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

void DependencyInfo::UpdateVersion()
{
    SetVersion(QString::fromStdString(_getVersionFunction()));
}

bool DependencyInfo::IsUpdateAvailable()
{
    return _updateAvailable;
}