#include "MainScreenGUIManager.h"

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "YTMusicAPI.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    
    MainScreenGUIManager manager;
    engine.rootContext()->setContextProperty("_manager", &manager);

    engine.load(QUrl("qrc:/gui/main.qml"));

    YTMusicAPI yt;
    yt.Search("NEWLY HUMAN FEELING", "songs", 10);

    return app.exec();
}