#ifndef QEMPLOYEESHIFTSWEEKLYREPORT_H
#define QEMPLOYEESHIFTSWEEKLYREPORT_H

#include <QWidget>

namespace Ui {
class QEmployeeShiftsWeeklyReport;
}

class QEmployeeShiftsWeeklyReport : public QWidget
{
    Q_OBJECT

public:
    explicit QEmployeeShiftsWeeklyReport(QWidget *parent = 0);
    ~QEmployeeShiftsWeeklyReport();

private:
    Ui::QEmployeeShiftsWeeklyReport *ui;
};

#endif // QEMPLOYEESHIFTSWEEKLYREPORT_H
