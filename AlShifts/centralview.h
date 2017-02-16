#ifndef CENTRALVIEW_H
#define CENTRALVIEW_H

#include <QWidget>
#include <QStackedWidget>

class QEmployeeShiftsTable;
class LegendForm;

class CentralView : public QWidget
{
    Q_OBJECT
public:
    explicit CentralView(QWidget *parent = 0);

    QEmployeeShiftsTable *employeeShiftsTable() {return m_emplTable;  }
    QStackedWidget *cvStackedWnd() { return m_stackWnd; }

signals:

public slots:
    void populate();

private:
    QEmployeeShiftsTable *m_emplTable;
    QStackedWidget *m_stackWnd;
    LegendForm *m_legendForm;
};

#endif // CENTRALVIEW_H
