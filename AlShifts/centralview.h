#ifndef CENTRALVIEW_H
#define CENTRALVIEW_H

#include <QWidget>
#include <QStackedWidget>

class QEmployeeShiftsTable;

class CentralView : public QWidget
{
    Q_OBJECT
public:
    explicit CentralView(QWidget *parent = 0);

signals:

public slots:

private:
    QEmployeeShiftsTable *m_emplTable;
    QStackedWidget *m_stackWnd;
};

#endif // CENTRALVIEW_H
