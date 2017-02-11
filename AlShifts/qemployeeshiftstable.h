#ifndef QEMPLOYEESHIFTSTABLE_H
#define QEMPLOYEESHIFTSTABLE_H

#include <QTableWidget>
#include "qshiftscore.h"

using Algorithmos::QEmployee;
using Algorithmos ::QShiftDay;


typedef QMap<Algorithmos::ShiftType, QVector<QEmployee*> > EmployeeMap;
typedef QMap<Algorithmos::ShiftType, QVector<QEmployee*> >::Iterator EmployeeMapIter;
typedef QVector<QShiftDay *> Shifts;

class QEmployeeShiftsTable : public QTableWidget
{
    Q_OBJECT
public:
    QEmployeeShiftsTable(QWidget *parent = Q_NULLPTR);
    ~QEmployeeShiftsTable();

    void populate();

private:
    QMap<int,int> m_eRow; //Contains id - row pairs for employees.
    void populateVHeader(EmployeeMap &e_map);
    void populateShiftsTable(Shifts &shifts);
};

#endif // QEMPLOYEESHIFTSTABLE_H
