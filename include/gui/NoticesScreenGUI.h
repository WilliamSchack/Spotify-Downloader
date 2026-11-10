#ifndef NOTICESSCREENGUI_H
#define NOTICESSCREENGUI_H

#include <QObject>

class NoticesScreenGUI : public QObject
{
    Q_OBJECT

    public:
        explicit NoticesScreenGUI(QObject* parent = 0);
};

#endif