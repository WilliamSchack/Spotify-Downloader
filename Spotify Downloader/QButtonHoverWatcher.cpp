#include "QButtonHoverWatcher.h"

QButtonHoverWatcher::QButtonHoverWatcher(QObject* parent) : QObject(parent) {}

void QButtonHoverWatcher::AddButtonFunctions(QPushButton* button, std::function<void(QPushButton*)> onHover, std::function<void(QPushButton*)> onEndHover) {
    HoverFunctions functions = {
        onHover,
        onEndHover
    };

    _hoverFunctions.insert(button, functions);

    button->installEventFilter(this);
}

bool QButtonHoverWatcher::eventFilter(QObject* watched, QEvent* event)
{
    QPushButton* button = qobject_cast<QPushButton*>(watched);
    if (!button)
        return false;

    if (!_hoverFunctions.contains(button))
        return false;

    HoverFunctions functions = _hoverFunctions[button];

    if (event->type() == QEvent::Enter) {
        // The push button is hovered by mouse
        functions.OnHover(button);
        return true;
    }

    if (event->type() == QEvent::Leave) {
        // The push button is not hovered by mouse
        functions.OnEndHover(button);
        return true;
    }

    return false;
}