#define QT_MESSAGELOGCONTEXT
#include "Utilities/Logger.h"

#include "SpotifyDownloader.h"
#include "Application.h"

int main(int argc, char *argv[])
{
    Logger::init();

    Application a(argc, argv);
    SpotifyDownloader w;
    w.show();
    bool state = a.exec();
    
    qInfo() << "Application Quit";

    Logger::Flush();
    Logger::clean();
    return state;
}