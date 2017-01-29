#pragma once
#include "qshiftscore_global.h"

namespace Algorithmos {

    enum QSHIFTSCORE_EXPORT EmployeeType { BMANAGER, BFUELMANAGER, FUELMANAGER ,INVALID};
    enum QSHIFTSCORE_EXPORT ShiftType { EARLY, LATE, INTERMITTENT,DAYOFF };


    QString EmployeeTypeName(EmployeeType type);
    EmployeeType setEmployeeTypeByName(const QString &name);
    QString to_qstring(const std::string &s);
    EmployeeType parseEmployeeType(const QString &str_type);
}
