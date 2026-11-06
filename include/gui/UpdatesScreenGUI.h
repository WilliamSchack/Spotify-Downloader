#ifndef UPDATESSCREENGUI_H
#define UPDATESSCREENGUI_H

#include <QObject>

class UpdatesScreenGUI : public QObject
{
    Q_OBJECT

    public:
        explicit UpdatesScreenGUI(QObject* parent = 0);
};

#endif