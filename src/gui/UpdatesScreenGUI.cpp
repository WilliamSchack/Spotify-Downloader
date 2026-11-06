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

void UpdatesScreenGUI::CreateDependency(const DependencyInfo::EDependency& type, const QString& name, const QString& version, const QString& iconSource)
{
    DependencyInfo* echoliteInfo = new DependencyInfo(this);
    echoliteInfo->SetType(type);
    echoliteInfo->SetName(name);
    echoliteInfo->SetIconSource(iconSource);
    echoliteInfo->SetVersion(version);
    _dependencies.insert(type, echoliteInfo);
}

void UpdatesScreenGUI::CheckForUpdatesButtonClicked()
{
    SetCheckingForUpdates(true);
    SetUpdateButtonText("Checking for Updates...");

    // Dont stall the gui thread
    // TODO: Cleanup when changing between dependencies
    DependencyInfo::EDependency type = _selectedDependencyType;
    std::thread([this, type]() {
        bool updateAvailable = false;
        switch (type) {
            case DependencyInfo::EDependency::Echolite: {
                updateAvailable = SelfUpdater::UpdateAvailable();
                break;
            } case DependencyInfo::EDependency::Ffmpeg: {
                updateAvailable = Ffmpeg::UpdateAvailable();
                break;
            } case DependencyInfo::EDependency::Ytdlp: {
                updateAvailable = Ytdlp::UpdateAvailable();
                break;
            } case DependencyInfo::EDependency::Quickjs: {
                updateAvailable = JsRuntime::UpdateAvailable();
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