#include "Application.h"

Application::Application(int& argc, char** argv) : QApplication(argc, argv) { }

bool Application::notify(QObject* receiver, QEvent* event) {
    try {
        return QApplication::notify(receiver, event);
    }
    catch (std::exception& e) {
        qFatal("Error %s sending event %s to object %s (%s)",
            e.what(), typeid(*event).name(), qPrintable(receiver->objectName()),
            typeid(*receiver).name());
    }
    catch (...) {
        qFatal("Error <unknown> sending event %s to object %s (%s)",
            typeid(*event).name(), qPrintable(receiver->objectName()),
            typeid(*receiver).name());
    }

    return false;
}