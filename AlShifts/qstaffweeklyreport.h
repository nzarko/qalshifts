#ifndef QSTAFFWEEKLYREPORT_H
#define QSTAFFWEEKLYREPORT_H

#include <QTableWidget>
#include <QDate>
#include <QMap>
#include <QAction>
#include <weekreportinfo.h>

class QEmployeeShiftsTable;

class QStaffWeeklyReport : public QTableWidget
{
    Q_OBJECT
public:
    QStaffWeeklyReport(QEmployeeShiftsTable *empl,QWidget *parent = Q_NULLPTR);
    ~QStaffWeeklyReport();

public Q_SLOTS:
    void createWeekReport(const QDate &date);
    void setCSVFilename(const QString &filename);
    void openWithExcel();

private:
    struct WeekDaysRow {
        int startRow;
        int span;
    };

    QAction *openWithExcelAction;
    QMap <int, WeekDaysRow> weekDaysRow;
    QMap <int, WeekDaysRow> fuelweekDaysRow;
    QVector<WeekReportInfo> wriVec;
    QDate weekDate;
    QString csvFilename;
    QEmployeeShiftsTable *pEST;
    QMap<QString, int> brColumn;
    int startRow;
    void init(const QDate &date);
    void createSkeleton(const QDate &date,bool forFuelEmployees = false);
    void getWeekReportInfo(QVector<WeekReportInfo> &wri_vec);
    float dayWorkHours(int day, Algorithmos::ShiftType shiftType);
    void getWeekWorkHours(QVector<WeekReportInfo> &wri_vec, QMap<QString ,float> &weekWH);
    void  refineRows();
    bool writeCSVFile(const QString &filename);
};

#endif // QSTAFFWEEKLYREPORT_H
