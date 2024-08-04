#define QT_MESSAGELOGCONTEXT
#include "Logger.h"

#include "Application.h"
#include "SpotifyDownloader.h"

int main(int argc, char *argv[])
{
    Logger::init();
    qInfo() << "RUNNING VERSION" << SpotifyDownloader::VERSION;

    Application a(argc, argv);
    SpotifyDownloader w;
    w.show();
    bool state = a.exec();
    
    qInfo() << "Application Quit";

    Logger::Flush();
    Logger::clean();
    return state;
}