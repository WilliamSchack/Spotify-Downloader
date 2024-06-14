#include "ObjectHoverWatcher.h"

ObjectHoverWatcher::ObjectHoverWatcher(QObject* parent) : QObject(parent) {}

void ObjectHoverWatcher::AddObjectFunctions(QObject* object, std::function<void(QObject*)> onHover, std::function<void(QObject*)> onEndHover) {
    HoverFunctions functions = {
        onHover,
        onEndHover
    };

    _hoverFunctions.insert(object, functions);

    object->installEventFilter(this);
}

bool ObjectHoverWatcher::eventFilter(QObject* watched, QEvent* event) {
    QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

    //QPushButton* button = qobject_cast<QPushButton*>(watched);
    //if (!button)
    //    return false;

    //if (!_hoverFunctions.contains(button))
    //    return false;

    //qDebug() << watched->objectName() << event->type() << mouseEvent->type();

    HoverFunctions functions = _hoverFunctions[watched];

    switch (event->type()) {
        case QEvent::Enter:
            if (functions.OnHover != Q_NULLPTR) {
                functions.OnHover(watched);
                //return true;
            }

            break;
        case QEvent::Leave:
            if (functions.OnEndHover != Q_NULLPTR) {
                functions.OnEndHover(watched);
                //return true;
            }

            break;
    }

    return false;
}