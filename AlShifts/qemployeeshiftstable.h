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

    /**
     * @brief The BranchSolverData struct
     * Contains information and data for the solver
     */
    struct BranchSolverData {
        int row;
        int col;
        QStringList branches;
    };

    /**
      * @brief The EmployeeTypeRowRange struct
      * Contains the start and the end row for each type
      * of employees in the table.
      *
      */

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

    /**
     * @brief createSolverData Create' solver data for all employees
     * @param range The range (in rows) for each type of employees.
     * @param col The column we want to create solver data
     * @param shift_type The shift type we want to create the data to solve
     * @return
     */
    StringListArray createSolverData(ETRange range, int col, int shift_type);

    /**
     * @brief createEmployeesSolverData Create's data for fuel employees
     * @param shift_type Shift type to create data
     * @return A StringListArray containing the data to solve.
     *
     * Make sure to call this function after @link createSolverData is called.
     */
    StringListArray createEmployeesSolverData(int col,int shift_type);

    /**
     * @brief setStartDate Set the start date to horizontal header items.
     * @param dt Starter date.
     */
    void setStartDate(QDateTime dt);

    QString currentLocation();

public slots:
    void clearShifts();
    void print();
    void printPreview();
    QString exportToHtml();
    void solve();
    void solve(int col);
    void updateCell(int row, int col);
    void updateItemColumn(QTableWidgetItem *item);

    /**
     * @brief swapShifts Change shifts between cells.
     * We change only the data (STIROLE) and nothing else.
     * @param item1 The first of two items
     * @param item2 The second item
     */
    void swapShifts(QTableWidgetItem *item1, QTableWidgetItem *item2);
    /**
     * @brief updateActions Solve current column if the user select two
     * different types of shifts and swap between them .
     */
    void updateActions();
    void swapShifts();
    void rearrangeEmployeesShift();
    void loadBFuelShifts();

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
