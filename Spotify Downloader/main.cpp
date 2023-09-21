#include "SpotifyDownloader.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SpotifyDownloader w;
    w.show();
    return a.exec();
}