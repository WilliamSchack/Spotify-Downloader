#ifndef SUPPORTERSECTION_H
#define SUPPORTERSECTION_H

#include "Supporter.h"

#include <QObject>
#include <QList>

class SupporterSection
{
    Q_GADGET
    Q_PROPERTY(QString title READ GetTitle CONSTANT)
    Q_PROPERTY(QList<Supporter> supporters READ GetSupporters CONSTANT)

    public:
        QString GetTitle()               const { return Title; }
        QList<Supporter> GetSupporters() const { return Supporters; }

        QString Title = "";
        QList<Supporter> Supporters;
};

#endif