#include "UpdatesScreenGUI.h"

// TODO: Dont allow downloads if all dependencies are not installed

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
            // TODO: Implement update popup
            std::cout << "IMPLEMENT ME!" << std::endl;
            return false;
        },
        []() -> bool {
            return false;
        }
    );

    CreateDependency(
        DependencyInfo::EDependency::Ffmpeg,
        "ffmpeg",
        "",
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
        }
    );

    CreateDependency(
        DependencyInfo::EDependency::Ytdlp,
        "yt-dlp",
        "",
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
        }
    );

    CreateDependency(
        DependencyInfo::EDependency::Quickjs,
        "QuickJs",
        "",
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
        }
    );

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

void UpdatesScreenGUI::CreateDependency(const DependencyInfo::EDependency& type, const QString& name, const QString& iconSource, std::function<bool()> installedFunction, std::function<bool()> installedOnSystemFunction, std::function<std::string()> getVersionFunction, std::function<bool()> updateAvailableFunction, std::function<bool()> updateFunction, std::function<bool()> downloadFunction)
{
    DependencyInfo* echoliteInfo = new DependencyInfo(installedFunction, installedOnSystemFunction, getVersionFunction, updateAvailableFunction, updateFunction, downloadFunction, this);
    echoliteInfo->SetType(type);
    echoliteInfo->SetName(name);
    echoliteInfo->SetIconSource(iconSource);
    _dependencies.insert(type, echoliteInfo);
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