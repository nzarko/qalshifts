#ifndef QEMPLOYEESHIFTSTABLE_H
#define QEMPLOYEESHIFTSTABLE_H

#include <QTableWidget>
#include "qshiftscore.h"
#include "qshiftstableitem.h"

QT_BEGIN_NAMESPACE
class QPrinter;
class QPrintDialog;
class QPrintPreviewDialog;
QT_END_NAMESPACE

using Algorithmos::QShiftsTableItem;
using Algorithmos::QEmployee;
using Algorithmos ::QShiftDay;


typedef QMap<Algorithmos::ShiftType, QVector<QEmployee*> > EmployeeMap;
typedef QMap<Algorithmos::ShiftType, QVector<QEmployee*> >::Iterator EmployeeMapIter;
typedef QVector<QShiftDay *> Shifts;
typedef std::vector<std::string> StringList;
typedef std::vector<std::vector<std::string> > StringListArray;


class QEmployeeShiftsTable : public QTableWidget
{
    Q_OBJECT
public:
    QEmployeeShiftsTable(QWidget *parent = Q_NULLPTR);
    ~QEmployeeShiftsTable();

    void populate();
    bool isEmpty();

    struct BranchSolverData {
        int row;
        int col;
        QStringList branches;
    };

    typedef struct EmployeeTypeRowRange {
        int startRow;
        int endRow;
        EmployeeTypeRowRange() : startRow(0), endRow(0) {}
        EmployeeTypeRowRange(int s,int e) :startRow(s), endRow(e) {}
        EmployeeTypeRowRange operator +(const int k);
        EmployeeTypeRowRange operator -(const int k);
        EmployeeTypeRowRange &operator +=(const int k);
        EmployeeTypeRowRange &operator -=(const int k);
    } ETRange;

    StringListArray createSolverData(ETRange range, int col, int shift_type);
    void setStartDate(QDateTime dt);

public slots:
    void clearShifts();
    void print();
    void printPreview();
    QString exportToHtml();
    void solve();
    void solve(int col);
    void updateCell(int row, int col);
    void updateItemColumn(QTableWidgetItem *item);
    void swapShifts(QTableWidgetItem *item1, QTableWidgetItem *item2);
    void updateActions();
    void swapShifts();
private:
    QMap<int,int> m_eRow; //Contains id - row pairs for employees.
    QMap<int, QBrush> itemBgColor; //item background color depending on shift type.
    void populateVHeader(EmployeeMap &e_map);
    void populateVHeader(const QVector<QEmployee*> &em_vec);
    void populateShiftsTable(Shifts &shifts);
    void populateShiftsTable(const UBlas::matrix<int>& m);
    void fillTableByEmployeeCategory(EmployeeMap &m_map, int j);
    bool is_empty;
    static int r; //For population (keep track of current row during table population through employee types.
    BranchSolverData *bsdata;
    QVector<BranchSolverData*> bsdata_vec;
    /**
     * @brief set_r
     * Sets the value of static variable r to val
     * Mostly used when the user clear the tablwe contents.
     * @param val The new value of static r.
     */
    static void set_r(int val);

    Algorithmos::QShiftsCore s_core;
    Algorithmos::QShiftSolver *s_solver;
    QDateTime m_startDate;
    //Keep track of employee type start and end row in table.
    QMap<Algorithmos::EmployeeType, ETRange> emtypeRange;
    ETRange manRange, fmanRange,feRange;
    bool is_in_solve_fun;

signals:
    void populationChanged(bool);
};

#endif // QEMPLOYEESHIFTSTABLE_H
