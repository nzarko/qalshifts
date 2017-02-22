    #pragma once

#include <boost/numeric/ublas/matrix.hpp>
//#include <boost/numeric/ublas/io.hpp>
//#include <boost/spirit/include/karma.hpp>
#include <boost/multi_array.hpp>

#include <QObject>
#include <QDateTime>

#include "qemployee.h"
#include "utils.h"

namespace UBlas = boost::numeric::ublas;


namespace Algorithmos {
typedef QMap<Algorithmos::ShiftType, QVector<QEmployee*> > EmployeeMap;

/**
     * @brief The QShiftDay class
     * Represents a shift day.
     */
    class QSHIFTSCORE_EXPORT QShiftDay
    {
    public:
        EmployeeMap &bManagers();
        EmployeeMap & bfManagers();
        EmployeeMap & fEmployees();
        const QDateTime &shiftDate();
        const int reqManagers();
        const int reqFManagers();
        const int reqFEmployees();

        QShiftDay();
        QShiftDay(const QDateTime &dt);
        QShiftDay(const QDateTime &dt, EmployeeMap &bm,
            EmployeeMap &bfm, EmployeeMap &fe);
        ~QShiftDay();

    private:
        QDateTime m_date;
        EmployeeMap  m_bManagers;
        EmployeeMap  m_bfManagers;
        EmployeeMap  m_fEmployees;
        int m_reqManagers, m_reqFManagers, m_reqFEmployees;
    };

    typedef QVector<QShiftDay *> Shifts;
    typedef QVector<QEmployee *> EmployeeGroup;

    class QSHIFTSCORE_EXPORT QShiftSolver : public QObject
    {
        Q_OBJECT

    public:
        QShiftSolver(QObject *parent = 0);
        QShiftSolver(EmployeeGroup &man, EmployeeGroup &fman, EmployeeGroup &fe, QObject *parent = 0);
        ~QShiftSolver();
        void setEmployeeGroups(EmployeeGroup &man, EmployeeGroup &fman, EmployeeGroup &fe);
        int solve(EmployeeGroup &eg);
        Shifts & initShifts();
        Shifts &shifts();

        /**
         * @brief create_managers_shifts_matrix
         * Create the shifts matrix for managers and fuel managers
         * @return a 7x49 matrix with initial shifts. (Not the optimal)
         */
        UBlas::matrix<int> &create_managers_shifts_matrix();
        /**
         * @brief managersShiftsMatrix
         * @return the managers shifts matrix.
         */
        UBlas::matrix<int> &managersShiftsMatrix();

        /**
         * @brief fuelmanagersShiftsMatrix
         * @return the fuel managers shifts matrix.
         */
        UBlas::matrix<int> &fuelmanagersShiftsMatrix();

        /**
         * @brief create_employees_shifts_matrix
         * Create the shifts matrix for employees
         * @return a 8x49 matrix with initial shifts. (Not the optimal)
         */
        UBlas::matrix<int> &create_employees_shifts_matrix();

        /**
         * @brief employeeShiftsMatrix
         * @return the employee shifts matrix (may be optimal!)
         */
        UBlas::matrix<int> &employeeShiftsMatrix();
    private:
        QShiftDay *m_currentShift;
        Shifts m_pShifts;
        EmployeeGroup m_manGroup;
        EmployeeGroup m_fuelManGroup;
        EmployeeGroup m_fuelEmployeeGroup;
        UBlas::matrix<int> m_smatrix;   //Branch Managers
        UBlas::matrix<int> fm_smatrix; // Branch Fuel managers
        UBlas::matrix<int> ef_smatrix; // Branch Fuel Employees

        void init_matrix_with_zeros(UBlas::matrix<int> &mat);

        /**
         * @brief solve_managers Tries to place managers to proper shift
         * @return tha maximum number of constraints have not been met.
         */
        int solve_managers();
        /**
         * @brief count_col_zeros
         * @param mat Shifts matrix
         * @param j matrix column
         * @return the number of zeros exists in column j
         */
        int count_col_zeros(const UBlas::matrix<int> &mat, size_t j);

        /**
         * @brief find003
         * Find the pattern 003 in neighour columns in matrix mat
         * @param mat Shifts Matrix
         * @param j matrix column
         * @return -1 if the pattern does not exists or the
         * number of row where the first zero exists.
         */
        int find003(const UBlas::matrix<int> &mat, size_t j);
    };

    QDebug operator<<(QDebug debug, const UBlas::matrix<int> &mat);

}
