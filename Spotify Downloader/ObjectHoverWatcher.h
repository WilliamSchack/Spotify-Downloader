#ifndef OBJECTHOVERWATCHER_H
#define OBJECTHOVERWATCHER_H

#include <QPushButton>
#include <QMap>
#include <QEvent>
#include <QMouseEvent>

class ObjectHoverWatcher : public QObject
{
    Q_OBJECT

    struct HoverFunctions {
        std::function<void(QObject*)> OnHover = Q_NULLPTR;
        std::function<void(QObject*)> OnEndHover = Q_NULLPTR;
    };

    public:
        explicit ObjectHoverWatcher(QObject* parent = Q_NULLPTR);
        void AddObjectFunctions(QObject* object, std::function<void(QObject*)> onHover, std::function<void(QObject*)> onEndHover);
        virtual bool eventFilter(QObject* watched, QEvent* event) Q_DECL_OVERRIDE;
    private:
        QMap<QObject*, HoverFunctions> _hoverFunctions;
};

#endif