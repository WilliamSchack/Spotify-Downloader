#include "UpdatesScreenGUI.h"

UpdatesScreenGUI::UpdatesScreenGUI(QObject* parent) : QObject(parent)
{
    qmlRegisterUncreatableType<DependencyInfo>("Echolite.Info.Updates", 1, 0, "DependencyInfo", "Created in c++");

    CreateDependency(
        DependencyInfo::EDependency::Echolite,
        "Echolite",
        "qrc:/icons/Icon.svg",
        []() -> bool {
            return true;
        },
        []() -> bool {
            return false;
        },
        []() -> std::string {
            return APP_VERSION;
        },
        []() -> bool {
            return VersionManager::IsUpdateAvailable(true);
        },
        []() -> bool {
            QDesktopServices::openUrl(QUrl("https://github.com/WilliamSchack/Echolite"));
            return false;
        },
        []() -> bool {
            return false;
        },
        [](int resultsPerPage, int page) -> QList<GUIReleaseInfo> {
            std::vector<GitHubRelease> releases = VersionManager::GetReleases(resultsPerPage, page);
            
            QList<GUIReleaseInfo> guiReleases;
            for (GitHubRelease release : releases) {
                GUIReleaseInfo guiRelease;
                guiRelease.Name = QString::fromStdString(release.Name);
                guiRelease.Body = QString::fromStdString(release.Body);

                guiReleases.push_back(guiRelease);
            }

            return guiReleases;
        }
    );

    CreateDependency(
        DependencyInfo::EDependency::Ffmpeg,
        "FFmpeg",
        "qrc:/icons/dependencies/FFmpeg.png",
        []() -> bool {
            return Ffmpeg::IsInstalled();
        },
        []() -> bool {
            return !Ffmpeg::IsInstalledRelative();
        },
        []() -> std::string {
            return Ffmpeg::GetVersion();
        },
        []() -> bool {
            return Ffmpeg::IsUpdateAvailable(true);
        },
        []() -> bool {
            return Ffmpeg::UpdateExecutable();
        },
        []() -> bool {
            return Ffmpeg::DownloadExecutable();
        },
        [](int resultsPerPage, int page) -> QList<GUIReleaseInfo> {
            std::vector<GitHubRelease> releases = Ffmpeg::GetReleases(resultsPerPage, page);
            
            QList<GUIReleaseInfo> guiReleases;
            for (GitHubRelease release : releases) {
                GUIReleaseInfo guiRelease;
                guiRelease.Name = QString::fromStdString(release.Name);
                guiRelease.Body = QString::fromStdString(release.Body);

                guiReleases.push_back(guiRelease);
            }

            return guiReleases;
        }
    );

    CreateDependency(
        DependencyInfo::EDependency::Ytdlp,
        "yt-dlp",
        "qrc:/icons/dependencies/yt-dlp.png",
        []() -> bool {
            return Ytdlp::IsInstalled();
        },
        []() -> bool {
            return !Ytdlp::IsInstalledRelative();
        },
        []() -> std::string {
            return Ytdlp::GetVersion();
        },
        []() -> bool {
            return Ytdlp::IsUpdateAvailable(true);
        },
        []() -> bool {
            return Ytdlp::UpdateExecutable();
        },
        []() -> bool {
            return Ytdlp::DownloadExecutable();
        },
        [](int resultsPerPage, int page) -> QList<GUIReleaseInfo> {
            std::vector<GitHubRelease> releases = Ytdlp::GetReleases(resultsPerPage, page);
            
            QList<GUIReleaseInfo> guiReleases;
            for (GitHubRelease release : releases) {
                GUIReleaseInfo guiRelease;
                guiRelease.Name = QString::fromStdString(release.Name);
                guiRelease.Body = QString::fromStdString(release.Body);

                guiReleases.push_back(guiRelease);
            }

            return guiReleases;
        }
    );

    CreateDependency(
        DependencyInfo::EDependency::Quickjs,
        "QuickJs",
        "qrc:/icons/dependencies/quickjs.png", // QuickJs has no icon, this is bellard's github icon
        []() -> bool {
            return JsRuntime::IsInstalled();
        },
        []() -> bool {
            return !JsRuntime::IsInstalledRelative();
        },
        []() -> std::string {
            return JsRuntime::GetVersion();
        },
        []() -> bool {
            return JsRuntime::IsUpdateAvailable(true);
        },
        []() -> bool {
            return JsRuntime::UpdateExecutable();
        },
        []() -> bool {
            return JsRuntime::DownloadExecutable();
        },
        [](int resultsPerPage, int page) -> QList<GUIReleaseInfo> {
            std::vector<GitHubRelease> releases = JsRuntime::GetReleases(resultsPerPage, page);
            
            QList<GUIReleaseInfo> guiReleases;
            for (GitHubRelease release : releases) {
                GUIReleaseInfo guiRelease;
                guiRelease.Name = QString::fromStdString(release.Name);
                guiRelease.Body = QString::fromStdString(release.Body);

                guiReleases.push_back(guiRelease);
            }

            return guiReleases;
        }
    );

    CheckAllDependenciesInstalled();
    CheckAllDependenciesUpToDate();
    
    SelectDependency(DependencyInfo::EDependency::Echolite);
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

void UpdatesScreenGUI::CreateDependency(const DependencyInfo::EDependency& type, const QString& name, const QString& iconSource, std::function<bool()> installedFunction, std::function<bool()> installedOnSystemFunction, std::function<std::string()> getVersionFunction, std::function<bool()> updateAvailableFunction, std::function<bool()> updateFunction, std::function<bool()> downloadFunction, std::function<QList<GUIReleaseInfo>(int, int)> getReleasesFunction)
{
    DependencyInfo* dependencyInfo = new DependencyInfo(installedFunction, installedOnSystemFunction, getVersionFunction, updateAvailableFunction, updateFunction, downloadFunction, getReleasesFunction, this);
    dependencyInfo->SetType(type);
    dependencyInfo->SetName(name);
    dependencyInfo->SetIconSource(iconSource);
    _dependencies.insert(type, dependencyInfo);

    connect(dependencyInfo, &DependencyInfo::InstalledChanged, this, &UpdatesScreenGUI::CheckAllDependenciesInstalled);
    connect(dependencyInfo, &DependencyInfo::UpdateAvailableChanged, this, &UpdatesScreenGUI::CheckAllDependenciesUpToDate);
}

void UpdatesScreenGUI::CheckAllDependenciesInstalled()
{
    for (DependencyInfo* info : _dependencies) {
        if (!info->GetInstalled()) {
            SetAllInstalled(false);
            return;
        }
    }

    SetAllInstalled(true);
}

void UpdatesScreenGUI::CheckAllDependenciesUpToDate()
{
    for (DependencyInfo* info : _dependencies) {
        if (info->GetUpdateAvailable()) {
            SetAnyUpdateAvailable(true);
            return;
        }
    }

    SetAnyUpdateAvailable(false);
}

void UpdatesScreenGUI::CheckForUpdatesButtonClicked()
{
    DependencyInfo* info = _dependencies[_selectedDependencyType];

    if (!info->GetInstalled())
        info->Download();

    if (info->GetUpdateAvailable())
        info->DownloadUpdate();
    else
        info->CheckForUpdate();
}

void UpdatesScreenGUI::SourceButtonClicked()
{
    switch (_selectedDependencyType) {
        case DependencyInfo::EDependency::Echolite:
            QDesktopServices::openUrl(QUrl("https://github.com/WilliamSchack/Echolite"));
            break;
        case DependencyInfo::EDependency::Ffmpeg:
            QDesktopServices::openUrl(QUrl("https://ffmpeg.org"));
            break;
        case DependencyInfo::EDependency::Ytdlp:
            QDesktopServices::openUrl(QUrl("https://github.com/yt-dlp/yt-dlp"));
            break;
        case DependencyInfo::EDependency::Quickjs:
            QDesktopServices::openUrl(QUrl("https://bellard.org/quickjs"));
            break;
    }
}