#pragma once
#include "qshiftscore_global.h"


namespace Algorithmos {

typedef std::vector<std::string> StringList;
typedef std::vector<std::vector<std::string> > StringListArray;

    enum QSHIFTSCORE_EXPORT EmployeeType { BMANAGER, BFUELMANAGER, FUELMANAGER ,INVALID};
    //Do not change the order bellow.
    enum QSHIFTSCORE_EXPORT ShiftType { EARLY, LATE, INTERMITTENT,DAYOFF,AVAILABLE };
    enum QSHIFTSCORE_EXPORT DataFileID { EMPLOYEES, MMATRIX, FMMATRIX, FEMATRIX, DOREPORT };


    QString QSHIFTSCORE_EXPORT EmployeeTypeName(EmployeeType type);
    QString QSHIFTSCORE_EXPORT EmployeeTypeShortName(EmployeeType type);
    EmployeeType setEmployeeTypeByName(const QString &name);
    QString to_qstring(const std::string &s);
    EmployeeType QSHIFTSCORE_EXPORT parseEmployeeType(const QString &str_type);
    QString QSHIFTSCORE_EXPORT shiftName(ShiftType stype);
    QStringList sl_to_qsl(StringList &sl);
    StringList QSHIFTSCORE_EXPORT qsl_to_sl(QStringList qsl);
    StringListArray find_compinations_of(StringListArray &v);
    void QSHIFTSCORE_EXPORT writeMatrixFiles();
    QString QSHIFTSCORE_EXPORT getFilePath(const DataFileID &id);
}
