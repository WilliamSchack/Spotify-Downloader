#include "ArgumentParser.h"
#include "MainScreenGUIManager.h"

#include "DownloadManager.h"

#include "Ytdlp.h"

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
    // Calling before arg parser so qtwebengine can be initialised
    QApplication app(argc, argv);

    bool argsUsed = ArgumentParser::Parse(argc, argv);
    if (argsUsed) return 0;

    QQmlApplicationEngine engine;
    
    MainScreenGUIManager manager;
    engine.rootContext()->setContextProperty("_manager", &manager);

    engine.load(QUrl("qrc:/gui/main.qml"));

    //DownloadManager::Download("https://music.youtube.com/watch?v=kky1L2jzc8c", "/home/william/mnt/main/Music/Test");

    //DownloadManager::Download("https://open.spotify.com/playlist/6xetLvEBK7k13730fmA1yP?si=UBrtGqnmS32xVmgd0PWJ0g", "/home/william/mnt/main/Music/Test");
    //DownloadManager::Download("https://open.spotify.com/playlist/4E5YNtoIyQrhtGuHr3Eb9U?si=beead3b4ce3845ad", "/home/william/mnt/main/Music/PlaylistTest");
    //DownloadManager::Download("https://open.spotify.com/track/31ZfD4958k80aCeMl719KC?si=58cf0c29d95f4d88", "/home/william/mnt/main/Music");
    //DownloadManager::Download("https://open.spotify.com/track/4d1CObMrwGif0yDhphYeO1?si=c498829f2829498a", "/home/william/mnt/main/Music");
    //DownloadManager::Download("https://open.spotify.com/track/1BmrBper5i6UFr5QwNirWB?si=538ceb8f18d149dc", "/home/william/mnt/main/Music");
    //DownloadManager::Download("https://open.spotify.com/album/4GRPvwkbXfKXNGfrIJAGCE?si=BfZ1WHfnTkG_P8J9quPtIw", "/home/william/mnt/main/Music/AlbumTest");
    //DownloadManager::Download("https://open.spotify.com/album/1JLclTNvD2eXpJ0BiJXmpy?si=dbpaiLrmTnKj2VYxzsiLgg", "/home/william/mnt/main/Music/AlbumTest");
    //DownloadManager::Download("https://open.spotify.com/playlist/1P95wXqcWer0Lsw8Mr5CXL?si=e77e7abc77be4a9f", "/home/william/mnt/main/Music/AlbumTest");
    //DownloadManager::Download("https://open.spotify.com/playlist/4E5YNtoIyQrhtGuHr3Eb9U?si=0ea99e52fb094258", "/home/william/mnt/main/Music/PlaylistTest");
    //DownloadManager::Download("https://open.spotify.com/playlist/62MNXgDSeAT8DPUWhdUifC?si=6f6b66ac97c4407c", "/home/william/mnt/main/Music/PlaylistTest");

    return app.exec();

    return 0;
}