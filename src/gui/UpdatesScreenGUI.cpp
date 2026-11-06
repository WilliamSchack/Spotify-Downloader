#include "UpdatesScreenGUI.h"

UpdatesScreenGUI::UpdatesScreenGUI(QObject* parent) : QObject(parent)
{
    qmlRegisterUncreatableType<DependencyInfo>("Echolite.Info.Updates", 1, 0, "DependencyInfo", "Created in c++");

    CreateDependency(
        DependencyInfo::EDependency::Echolite,
        "Echolite",
        "v" + QString(APP_VERSION),
        "qrc:/icons/Icon.svg"
    );

    CreateDependency(
        DependencyInfo::EDependency::Ffmpeg,
        "ffmpeg",
        "v",
        ""
    );

    CreateDependency(
        DependencyInfo::EDependency::Ytdlp,
        "yt-dlp",
        "v",
        ""
    );

    CreateDependency(
        DependencyInfo::EDependency::Quickjs,
        "QuickJs",
        "v",
        ""
    );
}

void UpdatesScreenGUI::SelectDependency(const DependencyInfo::EDependency& dependency)
{
    _selectedDependencyType = dependency;
    emit SelectedDependencyChanged();
}

DependencyInfo* UpdatesScreenGUI::GetDependency(const DependencyInfo::EDependency& dependency) const
{
    return _dependencies.value(dependency, nullptr);
}

DependencyInfo* UpdatesScreenGUI::GetSelectedDependency() const
{
    return GetDependency(_selectedDependencyType);
}

void UpdatesScreenGUI::CreateDependency(const DependencyInfo::EDependency& type, const QString& name, const QString& iconSource, const QString& version)
{
    DependencyInfo* echoliteInfo = new DependencyInfo(this);
    echoliteInfo->SetType(type);
    echoliteInfo->SetName(name);
    echoliteInfo->SetIconSource(iconSource);
    echoliteInfo->SetVersion(version);
    _dependencies.insert(type, echoliteInfo);
}