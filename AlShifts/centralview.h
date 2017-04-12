#ifndef CENTRALVIEW_H
#define CENTRALVIEW_H

#include <QWidget>
#include <QStackedWidget>

class QEmployeeShiftsTable;
class LegendForm;
class QEmployeeShiftsWeeklyReport;
class QStaffWeeklyReport;

class CentralView : public QWidget
{
    Q_OBJECT
public:
    explicit CentralView(QWidget *parent = 0);

    QEmployeeShiftsTable *employeeShiftsTable() {return m_emplTable;  }
    QEmployeeShiftsWeeklyReport *employeeReportTable() { return m_reportWnd; }
    QStaffWeeklyReport *staffReportTable() { return m_staffReportTbl; }
    QStackedWidget *cvStackedWnd() { return m_stackWnd; }

signals:

public slots:
    void populate();

private:
    QEmployeeShiftsTable *m_emplTable;
    QStackedWidget *m_stackWnd;
    LegendForm *m_legendForm;
    QEmployeeShiftsWeeklyReport *m_reportWnd;
    QStaffWeeklyReport *m_staffReportTbl;
};

#endif // CENTRALVIEW_H
