#include "SettingsScreenGUI.h"

SettingsScreenGUI::SettingsScreenGUI(QObject* parent) : QObject(parent)
{
    connect(this, &SettingsScreenGUI::YoutubePOTokenChanged, this, &SettingsScreenGUI::UpdateYouTubeCookiesStatus);

    SetupFileName();
    SetupSubFolders();
    SetupLyricsFileName();
    SetupPlaylistFileName();

    SetupYouTubeCookiesWatcher();
    UpdateYouTubeCookiesStatus();
}

void SettingsScreenGUI::LeavingScreen()
{
    SaveFileName();
    SaveSubFolders();
    SaveLyricsFileName();
    SavePlaylistFileName();

    CleaupYouTubeCookiesWatcher();
}

bool SettingsScreenGUI::GetAllSettingsValid() const
{
    return _fileNameValid && _subFoldersValid && _lyricsFileNameValid && _playlistFileNameValid;
}

QString SettingsScreenGUI::GetYouTubeCookiesStatus() const
{
    return _youtubeCookiesStatus;
}

void SettingsScreenGUI::YouTubeCookiesOpenButtonClicked()
{
    CookieFile& cookieFile = YouTubeCookieFile::Get();
    cookieFile.CreateIfEmpty();

    std::filesystem::path filePath = cookieFile.GetPath();
    QString filePathQString = QString::fromStdString(FileUtils::PathToUtf8(filePath));
    QUrl filePathQUrl = QUrl::fromLocalFile(filePathQString);

    QDesktopServices::openUrl(filePathQUrl);
}

void SettingsScreenGUI::YouTubeCookiesPasteButtonClicked()
{
    QClipboard* clipboard = QGuiApplication::clipboard();
    QString clipboardString = clipboard->text();
    YouTubeCookieFile::Get().Overwrite(clipboardString.toStdString());

    UpdateYouTubeCookiesStatus();
}

void SettingsScreenGUI::YouTubeCookiesClearButtonClicked()
{
    YouTubeCookieFile::Get().Clear();

    UpdateYouTubeCookiesStatus();
}

void SettingsScreenGUI::YouTubePOTokenPasteButtonClicked()
{
    QClipboard* clipboard = QGuiApplication::clipboard();
    QString clipboardString = clipboard->text();
    Config::YouTubePOToken = clipboardString.toStdString();

    emit YoutubePOTokenChanged();
}

void SettingsScreenGUI::YouTubePOTokenClearButtonClicked()
{
    Config::YouTubePOToken = "";

    emit YoutubePOTokenChanged();
}

void SettingsScreenGUI::UpdateYouTubeCookiesStatus()
{
    CookieFile& cookieFile = YouTubeCookieFile::Get();
    std::string cookieString = cookieFile.Read();
    bool cookieValid = cookieFile.CookiesValid();

    std::cout << "Cookies: " << cookieString << std::endl;

    QString cookieStatus = "";
    if (cookieString.empty())
        cookieStatus = "No Cookies";
    else if (!cookieValid)
        cookieStatus = "Cookies Invalid";

    QString poTokenStatus = "";
    if (Config::YouTubePOToken.empty())
        poTokenStatus = "No PO Token";
    
    if (!cookieStatus.isEmpty() || !poTokenStatus.isEmpty()) {
        _youtubeCookiesStatus = "<font color=\"#ff6464\">Invalid: ";
        _youtubeCookiesStatus += cookieStatus + (poTokenStatus.isEmpty() ? "" : ", ");
        _youtubeCookiesStatus += poTokenStatus;
        _youtubeCookiesStatus += "</font>";
    } else {
        _youtubeCookiesStatus = "<font color=\"#18e770\">Cookies Valid</font>";
    }

    emit YouTubeCookiesStatusChanged();
}

void SettingsScreenGUI::SetupYouTubeCookiesWatcher()
{
    connect(&_youtubeCookiesWatcher, &QFileSystemWatcher::fileChanged, this, &SettingsScreenGUI::YouTubeCookiesWatcherFileChanged);
    AddYouTubeCookiesWatcherPath();
}

void SettingsScreenGUI::AddYouTubeCookiesWatcherPath()
{
    std::filesystem::path filePath = YouTubeCookieFile::Get().GetPath();
    QString filePathQString = QString::fromStdString(FileUtils::PathToUtf8(filePath));
    if (!_youtubeCookiesWatcher.files().contains(filePathQString)) {
        if (!_youtubeCookiesWatcher.addPath(filePathQString))
            std::cout << "Failed to watch cookie file at: " << filePath << std::endl;
    }
}

void SettingsScreenGUI::CleaupYouTubeCookiesWatcher()
{
    disconnect(&_youtubeCookiesWatcher, nullptr, this, nullptr);
    _youtubeCookiesWatcher.removePaths(_youtubeCookiesWatcher.files());
}

void SettingsScreenGUI::YouTubeCookiesWatcherFileChanged(const QString& path)
{
    AddYouTubeCookiesWatcherPath();
    YouTubeCookieFile::Get().ClearCache();

    UpdateYouTubeCookiesStatus();
}