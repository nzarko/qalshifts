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
    bool isEmpty();

public slots:
    void clearShifts();

private:
    QMap<int,int> m_eRow; //Contains id - row pairs for employees.
    QMap<int, QBrush> itemBgColor; //item background color depending on shift type.
    void populateVHeader(EmployeeMap &e_map);
    void populateShiftsTable(Shifts &shifts);
    bool is_empty;
    static int r; //For population (keep track of current row during table population through employee types.
    /**
     * @brief set_r
     * Sets the value of static variable r to val
     * Mostly used when the user clear the tablwe contents.
     * @param val The new value of static r.
     */
    static void set_r(int val);

signals:
    void populationChanged(bool);
};

#endif // QEMPLOYEESHIFTSTABLE_H
