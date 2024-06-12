#ifndef QBUTTONHOVERWATCHER_H
#define QBUTTONHOVERWATCHER_H

#include <QPushButton>
#include <QMap>
#include <QEvent>

class QButtonHoverWatcher : public QObject
{
    Q_OBJECT

    struct HoverFunctions {
        std::function<void(QPushButton*)> OnHover;
        std::function<void(QPushButton*)> OnEndHover;
    };

    public:
        explicit QButtonHoverWatcher(QObject* parent = Q_NULLPTR);
        void AddButtonFunctions(QPushButton* button, std::function<void(QPushButton*)> onHover, std::function<void(QPushButton*)> onEndHover);
        virtual bool eventFilter(QObject* watched, QEvent* event) Q_DECL_OVERRIDE;
    private:
        QMap<QPushButton*, HoverFunctions> _hoverFunctions;
};

#endif