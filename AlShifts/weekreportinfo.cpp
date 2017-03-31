#include <QObject>

#include "weekreportinfo.h"

QString WeekReportInfo::workingHours(Algorithmos::ShiftType st, const QString &branchCode, int dayOfWeek, int monthOfYear)
{
    QString res;
    switch(st) {
    case Algorithmos::EARLY:
    {
        if(dayOfWeek <= 6)
            res = "6:30 - 14:30";
        else
            res = "7:00 - 14:30";
        break;
    }
    case Algorithmos::LATE :
    {
        res = "14:30 - 22:00";
        break;
    }
    case Algorithmos::INTERMITTENT:
    {
        if(monthOfYear >= 10 && monthOfYear <= 3) {
            if(branchCode == "BR2") {
                res = "8:00 - 16:00";
            } else if (branchCode == "BR3"|| branchCode == "BR5") {
                res = "8:00 - 13:00\n17:00 - 20:00";
            }
        } else {
            res = "8:00 - 13:00\n18:00-21:00";
        }
        break;
    }
    case Algorithmos::DAYOFF :
        res = QObject::tr("DO");
        break;
    case Algorithmos::AVAILABLE:
        res = "";
        break;
    }
    return res;
}

WeekReportInfo::WeekReportInfo()
{

}

QDebug operator<<(QDebug debug, const WeekReportInfo &wri)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << '(' << wri.name << ": " << wri.branches << ": "
                    << wri.shifts << ')' << endl;

    return debug;
}
