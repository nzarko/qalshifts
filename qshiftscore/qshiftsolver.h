    #pragma once

    #include <QObject>
    #include <QDateTime>

    #include "qemployee.h"
    #include "utils.h"


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
    private:
        QShiftDay *m_currentShift;
        Shifts m_pShifts;
        EmployeeGroup m_manGroup;
        EmployeeGroup m_fuelManGroup;
        EmployeeGroup m_fuelEmployeeGroup;

        /**
         * @brief solve_managers Tries to place managers to proper shift
         * @return tha maximum number of constraints have not been met.
         */
        int solve_managers();
    };
}
