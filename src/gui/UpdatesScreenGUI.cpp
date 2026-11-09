#include "UpdatesScreenGUI.h"

UpdatesScreenGUI::UpdatesScreenGUI(QObject* parent) : QObject(parent)
{
    qmlRegisterUncreatableType<DependencyInfo>("Echolite.Info.Updates", 1, 0, "DependencyInfo", "Created in c++");

    CreateDependency(
        DependencyInfo::EDependency::Echolite,
        "Echolite",
        "qrc:/icons/Icon.svg",
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
        }
    );

    CreateDependency(
        DependencyInfo::EDependency::Ffmpeg,
        "ffmpeg",
        "",
        []() -> std::string {
            return Ffmpeg::GetVersion();
        },
        []() -> bool {
            return Ffmpeg::IsUpdateAvailable(true);
        },
        []() -> bool {
            return false;
        }
    );

    CreateDependency(
        DependencyInfo::EDependency::Ytdlp,
        "yt-dlp",
        "",
        []() -> std::string {
            return Ytdlp::GetVersion();
        },
        []() -> bool {
            return Ytdlp::IsUpdateAvailable(true);
        },
        []() -> bool {
            return false;
        }
    );

    CreateDependency(
        DependencyInfo::EDependency::Quickjs,
        "QuickJs",
        "",
        []() -> std::string {
            return JsRuntime::GetVersion();
        },
        []() -> bool {
            return JsRuntime::IsUpdateAvailable(true);
        },
        []() -> bool {
            return false;
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

void UpdatesScreenGUI::CreateDependency(const DependencyInfo::EDependency& type, const QString& name, const QString& iconSource, std::function<std::string()> getVersionFunction, std::function<bool()> updateAvailableFunction, std::function<void()> updateFunction)
{
    DependencyInfo* echoliteInfo = new DependencyInfo(getVersionFunction, updateAvailableFunction, updateFunction, this);
    echoliteInfo->SetType(type);
    echoliteInfo->SetName(name);
    echoliteInfo->SetIconSource(iconSource);
    _dependencies.insert(type, echoliteInfo);
}

void UpdatesScreenGUI::CheckForUpdatesButtonClicked()
{
    SetCheckingForUpdates(true);

    // TODO: Set this in qml, makes more sense
    SetUpdateButtonText("Checking for Updates...");

    // Dont stall the gui thread
    // TODO: Cleanup when changing between dependencies
    DependencyInfo::EDependency type = _selectedDependencyType;
    std::thread([this, type]() {
        bool updateAvailable = false;
        switch (type) {
            case DependencyInfo::EDependency::Echolite: {
                updateAvailable = VersionManager::IsUpdateAvailable();
                break;
            } case DependencyInfo::EDependency::Ffmpeg: {
                updateAvailable = Ffmpeg::IsUpdateAvailable();
                break;
            } case DependencyInfo::EDependency::Ytdlp: {
                updateAvailable = Ytdlp::IsUpdateAvailable();
                break;
            } case DependencyInfo::EDependency::Quickjs: {
                updateAvailable = JsRuntime::IsUpdateAvailable();
                break;
            }
        }

        // Call gui stuff on the main thread
        QMetaObject::invokeMethod(this, [this, type, updateAvailable]() {
            // TODO: Have dict with what dependencies have updates, or just store it in the dependency object
            SetCheckingForUpdates(false);
            SetUpdateButtonText(updateAvailable ? "Download Update" : "Check for Updates");
        }, Qt::QueuedConnection);
    }).detach();
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