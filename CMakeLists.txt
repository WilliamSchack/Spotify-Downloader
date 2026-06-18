cmake_minimum_required(VERSION 3.24)

project(SpotifyDownloader VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "Minimum macOS deployment version")
endif()

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Network
    Widgets
    WebEngineCore
    WebEngineWidgets
    Concurrent
)

find_package(PkgConfig)
if(PkgConfig_FOUND)
    pkg_check_modules(TAGLIB IMPORTED_TARGET taglib)
endif()

if(NOT TARGET PkgConfig::TAGLIB)
    find_program(TAGLIB_CONFIG_EXECUTABLE taglib-config)
    if(NOT TAGLIB_CONFIG_EXECUTABLE AND EXISTS "/usr/local/opt/taglib/bin/taglib-config")
        set(TAGLIB_CONFIG_EXECUTABLE "/usr/local/opt/taglib/bin/taglib-config")
    endif()

    if(NOT TAGLIB_CONFIG_EXECUTABLE)
        message(FATAL_ERROR "TagLib was not found. Install TagLib or set taglib-config on PATH.")
    endif()

    execute_process(
        COMMAND "${TAGLIB_CONFIG_EXECUTABLE}" --cflags
        OUTPUT_VARIABLE TAGLIB_CFLAGS
        OUTPUT_STRIP_TRAILING_WHITESPACE
        COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(
        COMMAND "${TAGLIB_CONFIG_EXECUTABLE}" --libs
        OUTPUT_VARIABLE TAGLIB_LIBS
        OUTPUT_STRIP_TRAILING_WHITESPACE
        COMMAND_ERROR_IS_FATAL ANY
    )

    separate_arguments(TAGLIB_CFLAGS_LIST UNIX_COMMAND "${TAGLIB_CFLAGS}")
    separate_arguments(TAGLIB_LIBS_LIST UNIX_COMMAND "${TAGLIB_LIBS}")

    add_library(TagLib::TagLib INTERFACE IMPORTED)
    target_compile_options(TagLib::TagLib INTERFACE ${TAGLIB_CFLAGS_LIST})
    target_link_options(TagLib::TagLib INTERFACE ${TAGLIB_LIBS_LIST})
endif()

set(APP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Spotify Downloader")

set(SPOTIFY_DOWNLOADER_SOURCES
    "${APP_DIR}/main.cpp"
    "${APP_DIR}/Application.cpp"
    "${APP_DIR}/Config.cpp"
    "${APP_DIR}/SpotifyDownloader.cpp"
    "${APP_DIR}/Downloading/PlaylistDownloader.cpp"
    "${APP_DIR}/Downloading/Song.cpp"
    "${APP_DIR}/Downloading/SongDownloader.cpp"
    "${APP_DIR}/Lyrics/LRCFile.cpp"
    "${APP_DIR}/Lyrics/Lyrics.cpp"
    "${APP_DIR}/Network/MusixmatchAPI.cpp"
    "${APP_DIR}/Network/Network.cpp"
    "${APP_DIR}/Network/Notices.cpp"
    "${APP_DIR}/Network/SpotifyAPI.cpp"
    "${APP_DIR}/Network/SpotifyAuthInterceptor.cpp"
    "${APP_DIR}/Network/SpotifyAuthRetriever.cpp"
    "${APP_DIR}/Network/VersionManager.cpp"
    "${APP_DIR}/Network/YTMusicAPI.cpp"
    "${APP_DIR}/Playlist/M3UFile.cpp"
    "${APP_DIR}/Playlist/PLSFile.cpp"
    "${APP_DIR}/Playlist/PlaylistFile.cpp"
    "${APP_DIR}/Playlist/XSPFFile.cpp"
    "${APP_DIR}/UI/UISetup.cpp"
    "${APP_DIR}/UI/UIUtilities.cpp"
    "${APP_DIR}/UI/CustomWidgets/CheckBox.cpp"
    "${APP_DIR}/UI/CustomWidgets/DownloaderThread.cpp"
    "${APP_DIR}/UI/CustomWidgets/NoticeItem.cpp"
    "${APP_DIR}/UI/CustomWidgets/SongErrorItem.cpp"
    "${APP_DIR}/Utilities/Animation.cpp"
    "${APP_DIR}/Utilities/FileUtils.cpp"
    "${APP_DIR}/Utilities/ImageUtils.cpp"
    "${APP_DIR}/Utilities/JSONUtils.cpp"
    "${APP_DIR}/Utilities/Logger.cpp"
    "${APP_DIR}/Utilities/MathUtils.cpp"
    "${APP_DIR}/Utilities/ObjectHoverWatcher.cpp"
    "${APP_DIR}/Utilities/StringUtils.cpp"
    "${APP_DIR}/SpotifyDownloader.ui"
    "${APP_DIR}/SpotifyDownloader.qrc"
)

if(WIN32)
    list(APPEND SPOTIFY_DOWNLOADER_SOURCES "${APP_DIR}/SpotifyDownloader.rc")
endif()

qt_add_executable(SpotifyDownloader
    MACOSX_BUNDLE
    WIN32
    ${SPOTIFY_DOWNLOADER_SOURCES}
)

target_include_directories(SpotifyDownloader PRIVATE
    "${APP_DIR}"
    "${APP_DIR}/include"
)

target_link_libraries(SpotifyDownloader PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Network
    Qt6::Widgets
    Qt6::WebEngineCore
    Qt6::WebEngineWidgets
    Qt6::Concurrent
)

if(TARGET PkgConfig::TAGLIB)
    target_link_libraries(SpotifyDownloader PRIVATE PkgConfig::TAGLIB)
else()
    target_link_libraries(SpotifyDownloader PRIVATE TagLib::TagLib)
endif()

if(APPLE)
    set_target_properties(SpotifyDownloader PROPERTIES
        OUTPUT_NAME "SpotifyDownloader"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.williamschack.spotifydownloader"
        MACOSX_BUNDLE_BUNDLE_NAME "Spotify Downloader"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
    )
endif()

install(TARGETS SpotifyDownloader
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)
