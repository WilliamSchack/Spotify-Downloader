QT += core gui widgets network concurrent webenginecore webenginewidgets uitools

CONFIG += c++17
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15

TEMPLATE = app
TARGET = SpotifyDownloader

APP_DIR = "$$PWD/Spotify Downloader"

SOURCES += \
    "$$APP_DIR/main.cpp" \
    "$$APP_DIR/Application.cpp" \
    "$$APP_DIR/Config.cpp" \
    "$$APP_DIR/SpotifyDownloader.cpp" \
    "$$APP_DIR/Downloading/PlaylistDownloader.cpp" \
    "$$APP_DIR/Downloading/Song.cpp" \
    "$$APP_DIR/Downloading/SongDownloader.cpp" \
    "$$APP_DIR/Lyrics/LRCFile.cpp" \
    "$$APP_DIR/Lyrics/Lyrics.cpp" \
    "$$APP_DIR/Network/MusixmatchAPI.cpp" \
    "$$APP_DIR/Network/Network.cpp" \
    "$$APP_DIR/Network/Notices.cpp" \
    "$$APP_DIR/Network/SpotifyAPI.cpp" \
    "$$APP_DIR/Network/SpotifyAuthInterceptor.cpp" \
    "$$APP_DIR/Network/SpotifyAuthRetriever.cpp" \
    "$$APP_DIR/Network/VersionManager.cpp" \
    "$$APP_DIR/Network/YTMusicAPI.cpp" \
    "$$APP_DIR/Playlist/M3UFile.cpp" \
    "$$APP_DIR/Playlist/PLSFile.cpp" \
    "$$APP_DIR/Playlist/PlaylistFile.cpp" \
    "$$APP_DIR/Playlist/XSPFFile.cpp" \
    "$$APP_DIR/UI/UISetup.cpp" \
    "$$APP_DIR/UI/UIUtilities.cpp" \
    "$$APP_DIR/UI/CustomWidgets/CheckBox.cpp" \
    "$$APP_DIR/UI/CustomWidgets/DownloaderThread.cpp" \
    "$$APP_DIR/UI/CustomWidgets/NoticeItem.cpp" \
    "$$APP_DIR/UI/CustomWidgets/SongErrorItem.cpp" \
    "$$APP_DIR/Utilities/Animation.cpp" \
    "$$APP_DIR/Utilities/FileUtils.cpp" \
    "$$APP_DIR/Utilities/ImageUtils.cpp" \
    "$$APP_DIR/Utilities/JSONUtils.cpp" \
    "$$APP_DIR/Utilities/Logger.cpp" \
    "$$APP_DIR/Utilities/MathUtils.cpp" \
    "$$APP_DIR/Utilities/ObjectHoverWatcher.cpp" \
    "$$APP_DIR/Utilities/StringUtils.cpp"

HEADERS += \
    "$$APP_DIR/Application.h" \
    "$$APP_DIR/SpotifyDownloader.h" \
    "$$APP_DIR/Network/SpotifyAuthInterceptor.h" \
    "$$APP_DIR/UI/CustomWidgets/CheckBox.h" \
    "$$APP_DIR/UI/CustomWidgets/DownloaderThread.h" \
    "$$APP_DIR/UI/CustomWidgets/NoticeItem.h" \
    "$$APP_DIR/UI/CustomWidgets/SongErrorItem.h" \
    "$$APP_DIR/Utilities/ObjectHoverWatcher.h"

FORMS += "$$APP_DIR/SpotifyDownloader.ui"
RESOURCES += "$$APP_DIR/SpotifyDownloader.qrc"

INCLUDEPATH += \
    "$$APP_DIR" \
    "$$APP_DIR/include" \
    /usr/local/include

macx {
    QMAKE_LIBS_OPENGL = -framework OpenGL
    LIBS += -L/usr/local/lib -ltag
}

unix:!macx {
    LIBS += -ltag
}

win32 {
    RC_FILE = "$$APP_DIR/SpotifyDownloader.rc"
    LIBS += "$$APP_DIR/lib/tag.lib"
}
