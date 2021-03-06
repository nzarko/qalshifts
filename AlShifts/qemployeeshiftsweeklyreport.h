#ifndef QEMPLOYEESHIFTSWEEKLYREPORT_H
#define QEMPLOYEESHIFTSWEEKLYREPORT_H

#include <QWidget>
#include <QLocale>
#include <QMap>
#include <QVector>

namespace Ui {
class QEmployeeShiftsWeeklyReport;
}

QT_BEGIN_NAMESPACE
class QAction;
class QDate;
class QTableWidget;
class QTableWidgetItem;
class QAbstractItemModel;
class QPrinter;
QT_END_NAMESPACE

class QEmployeeShiftsTable;
class HeaderView;
class HeaderDelegate;

class QEmployeeShiftsWeeklyReport : public QWidget
{
    Q_OBJECT

public:
    explicit QEmployeeShiftsWeeklyReport(QEmployeeShiftsTable *empl,QWidget *parent = 0);
    ~QEmployeeShiftsWeeklyReport();

    void createWeekReport(int week);
    void createWeekReport(const QDate &date);
    void updateWeekReport(const QString &brName);
    void updateWeekReport(int branch);
    QAbstractItemModel *tableModel() const;
    void setCVSFilename(QString fileName) { cvsFilename = fileName ; }

public slots:
    void branchChanged(const QString &branchID);
    void print();
    void printTable(QPrinter* printer);
    void insertEmployeeRow();
    void removeEmployeeRow();
    void writeCSVFile(const QString &fileName);
    void openWithExcel();
private:
    QLocale el_locale;
    QString cvsFilename;
    QAction *insertRowAction;
    QAction *removeRowAction;
    QAction *openWithExcelAction;
    HeaderDelegate *headerDelegate;
    HeaderView *verticalHeaderView;
    QVector <int> hidden_rows;
    Ui::QEmployeeShiftsWeeklyReport *ui;
    QEmployeeShiftsTable *pEST;
    QMap<QString, QString> branchFullName;
    QTableWidget *weekTable;

    QTableWidgetItem *wItem(int row, int col);
    QMap<QString, int> &getBranchLines(int &sr, const QStringList &branches, QMap<QString, int> &bl);
    void initWeekTable();
    void hideRows();
    void restoreRows();
};

#endif // QEMPLOYEESHIFTSWEEKLYREPORT_H
