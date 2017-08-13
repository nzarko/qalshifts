#ifndef MANAGERSDOREPORTDIALOG_H
#define MANAGERSDOREPORTDIALOG_H

#include <QDialog>
#include <QUrl>

namespace Ui {
class ManagersDOReportDialog;
}

class QEmployeeShiftsTable;

class ManagersDOReportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ManagersDOReportDialog(QEmployeeShiftsTable *shiftsTbl,QWidget *parent = 0);
    ~ManagersDOReportDialog();
    QString do_report() { return reportStr; }
    QUrl getReportURL() const { return htmlFileURL; }

private:
    Ui::ManagersDOReportDialog *ui;
    QEmployeeShiftsTable *m_shiftsView;

    void createReport();
    QString reportStr;
    QUrl htmlFileURL;

private Q_SLOTS:
#if defined (QWEBENGINEPAGE_PRINT)
    void print();
#endif
};

#endif // MANAGERSDOREPORTDIALOG_H
