#ifndef SUPPORTER_H
#define SUPPORTER_H

#include <QObject>

#include <iostream>

class Supporter
{
    Q_GADGET
    Q_PROPERTY(QString name READ GetName CONSTANT)
    Q_PROPERTY(QString timeString READ GetTimeString CONSTANT)

    public:
        QString GetName()       const { std::cout << "NAME: " << Name.toStdString() << std::endl; return Name; }
        QString GetTimeString() const { return TimeString; }

        QString Name = "";
        QString TimeString = "";
};

#endif