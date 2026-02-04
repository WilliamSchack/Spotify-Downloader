#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "MainScreenGUIManager.h"

#include "NetworkRequest.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    
    MainScreenGUIManager manager;
    engine.rootContext()->setContextProperty("_manager", &manager);

    engine.load(QUrl("qrc:/gui/main.qml"));

    return app.exec();
}