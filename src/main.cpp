#include "MainScreenGUIManager.h"

#include "DownloadManager.h"
#include "SpotifyAPINew.h"

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

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
// > Amount of albums
// > Amount of tracks (honestly worse in all ways, takes more time and looses the disc number)
// Episode I dont know (havent looked into it)

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    
    MainScreenGUIManager manager;
    engine.rootContext()->setContextProperty("_manager", &manager);

    engine.load(QUrl("qrc:/gui/main.qml"));

    


    return app.exec();
}