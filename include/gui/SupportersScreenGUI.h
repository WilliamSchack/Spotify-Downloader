#ifndef SUPPORTERSSCREENGUI_H
#define SUPPORTERSSCREENGUI_H

#include <QObject>

class SupportersScreenGUI : public QObject
{
    Q_OBJECT

    public:
        explicit SupportersScreenGUI(QObject* parent = 0);
};

#endif