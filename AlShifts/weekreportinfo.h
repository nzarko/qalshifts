#ifndef WEEKREPORTINFO_H
#define WEEKREPORTINFO_H

#include <QString>
#include <QVector>
#include <QDebug>

#include "utils.h"
#include "qemployeeshiftstable.h"


struct WeekReportInfo
{
public:
    QString name;
    QStringList branches;
    QVector<Algorithmos::ShiftType> shifts;
    QString workingHours(Algorithmos::ShiftType, const QString &branchCode,
                         int dayOfWeek, int monthOfYear);
    WeekReportInfo();
};

QDebug operator<<(QDebug debug, const WeekReportInfo &wri);


#endif // WEEKREPORTINFO_H
