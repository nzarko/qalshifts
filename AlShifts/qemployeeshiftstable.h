#ifndef QEMPLOYEESHIFTSTABLE_H
#define QEMPLOYEESHIFTSTABLE_H

#include <QTableWidget>

class QEmployeeShiftsTable : public QTableWidget
{
    Q_OBJECT
public:
    QEmployeeShiftsTable(QWidget *parent = Q_NULLPTR);
    ~QEmployeeShiftsTable();
};

#endif // QEMPLOYEESHIFTSTABLE_H
