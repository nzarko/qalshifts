#include <QObject>
#include <QTextCodec>
#include <QDebug>

#include "utils.h"

namespace Algorithmos {
    QString EmployeeTypeName(EmployeeType type)
    {
        QString name;
        switch (type)
        {
        case Algorithmos::BMANAGER:
            name = QObject::tr("Branch Manager");
            break;
        case Algorithmos::BFUELMANAGER:
            name = QObject::tr("Branch Fuel Manager");
            break;
        case Algorithmos::FUELMANAGER:
            name = QObject::tr("Fuel Employee");
            break;
        }
        return name;
    }

    EmployeeType setEmployeeTypeByName(const QString & name)
    {
        EmployeeType type;
        if (name == "YK")
            type = Algorithmos::BMANAGER;
        else if (name == "YB")
            type = Algorithmos::BFUELMANAGER;
        else if (name == "B")
            type = Algorithmos::FUELMANAGER;
        else if (name == "YB-YK")
            type = Algorithmos::BFUELMANAGER;
        else {
            type = Algorithmos::INVALID;
            qDebug() << "Could not parse employee type. Check for typo in you employees file." << endl;
        }

        return type;
    }

    QString to_qstring(const std::string & s)
    {
        QByteArray arr = s.c_str();
        QTextCodec *codec = QTextCodec::codecForName("Windows-1253");
        return codec->toUnicode(arr);
    }

    EmployeeType parseEmployeeType(const QString &str_type)
    {
        EmployeeType type;
        if (str_type == "YK")
            type = Algorithmos::BMANAGER;
        else if (str_type == "YB")
            type = Algorithmos::BFUELMANAGER;
        else if (str_type == "B")
            type = Algorithmos::FUELMANAGER;
        else if (str_type == "YB-YK")
            type = Algorithmos::BFUELMANAGER;
        else{
            type = Algorithmos::INVALID;
            qDebug() << "Could not parse employee type. Check for typo in you employees file." << endl;
            qDebug() << "From employees file I get : " << str_type << " as type. Is this correct??" << endl;
        }

        return type;
    }

    QString shiftName(ShiftType stype)
    {
        QString name;
        switch(stype) {
        case EARLY:
            name = QObject::tr("Early");
            break;
        case LATE:
            name = QObject::tr("Late");
            break;
        case DAYOFF:
            name = QObject::tr("Day Off");
            break;
        case INTERMITTENT:
            name = QObject::tr("Intermittent");
            break;
        default:
            name = "Unknown";
        }
        return name;

    }

}
