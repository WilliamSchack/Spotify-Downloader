#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "MainScreenGUIManager.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    
    MainScreenGUIManager manager;
    manager.SetQmlContext(engine.rootContext());
    
    engine.rootContext()->setContextProperty("_manager", &manager);

    engine.load(QUrl("qrc:/gui/main.qml"));

    return app.exec();
}