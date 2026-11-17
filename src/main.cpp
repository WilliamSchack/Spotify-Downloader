#include "Logger.h"
#include "ArgumentParser.h"
#include "GUIManager.h"
#include "Config.h"

#include <random>

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>

#include <QtQuick/QQuickWindow>
#include <QWKQuick/qwkquickglobal.h>

// TODO: IMPLEMENT THIS:
//
// When getting a playlist, instead of fetching each track individually for its details (because we get fuck all from the playlist site),
// Get its album details and cache all the songs in it.
// That way we get all the details for each song (+ disc number which we dont get from the track itself, - release date, we will use the album release date)
// and in the case another song from that album exists in the playlist we dont need to do another web request since its already cached
// (Which in most cases its likely to have multiple songs from the same album so this should help a bit)
// 
// This is honestly the only optimisation I can think of in terms of minimising web requests for playlists
// because of how little information we get from that site
// 
// == Web requests:
// Track:
// > 1 (-disc number)
// > 2 (to add disc number)
// Album:
// > 1
// > Track Amount (To get exact release dates, which is not worth it since in most cases they are released on the same day as the album)
// Playlist
// > Amount of tracks / 100 (ceil) (-release date)
// > Amount of albums (to add release date)
// Episode I dont know (havent looked into it)

// TODO: Clear downloading folder before starting
// - Make sure no other downloads are happening, dont want to delete a download in progress
// - Also might be hard to check if any other instances of the other app are open and potentially downloading
// - Since its in a temp folder might just be a good idea to leave it and have a button to clear it
// - Will only have files there if the app crashes during a download so its not a big issue

int main(int argc, char** argv)
{
    Logger::Init();

    std::ostringstream versionSs;
    versionSs << "Running version " << APP_VERSION << " on ";
#ifdef IS_ARM_CPU
    versionSs << "ARM ";
#else
    versionSs << "x64 ";
#endif

#ifdef _WIN32
    versionSs << "Windows";
#elif __APPLE__
    versionSs << "Mac";
#else
    versionSs << "Linux";
#endif

    Logger::Info(versionSs.str());

    std::srand(std::time(NULL));

    bool argsUsed = ArgumentParser::ParseBeforeGUI(argc, argv);
    if (argsUsed) return 0;

    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/icons/Icon.png"));

    QFont appFont = app.font();
    int fontId = QFontDatabase::addApplicationFont(":/fonts/Roboto.ttf");
    appFont.setFamily(QFontDatabase::applicationFontFamilies(fontId).at(0));
    app.setFont(appFont);

    argsUsed = ArgumentParser::ParseAfterGUI(argc, argv);
    if (argsUsed) return 0;

    Config::Load();

    GUIManager manager;
    QQmlApplicationEngine engine;
    QWK::registerTypes(&engine);
    engine.rootContext()->setContextProperty("_manager", &manager);
    engine.loadFromModule(APP_NAME, "Main");

    int output = app.exec();

    Logger::Info("Exiting");
    Logger::Flush();

    return output;
}