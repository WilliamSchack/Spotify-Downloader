#include "DependencyInfo.h"

DependencyInfo::DependencyInfo(std::function<std::string()> getVersionFunction, std::function<bool()> updateAvailableFunction, std::function<void()> updateFunction, QObject* parent)
    : QObject(parent), _getVersionFunction(getVersionFunction), _updateAvailableFunction(updateAvailableFunction), _updateFunction(updateFunction)
{
    _version = QString::fromStdString(_getVersionFunction());
}