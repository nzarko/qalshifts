#include "qshiftsolver.h"
namespace Algorithmos {


    EmployeeMap & QShiftDay::bManagers()
    {
        // TODO: insert return statement here
        return m_bManagers;
    }

    EmployeeMap & QShiftDay::bfManagers()
    {
        // TODO: insert return statement here
        return m_bfManagers;
    }

    EmployeeMap & QShiftDay::fEmployees()
    {
        // TODO: insert return statement here
        return m_fEmployees;
    }

    const QDateTime &QShiftDay::shiftDate()
    {
        return m_date;
    }

    const int QShiftDay::reqManagers()
    {
        return m_reqManagers;
    }

    const int QShiftDay::reqFManagers()
    {
        return m_reqFManagers;
    }

    const int QShiftDay::reqFEmployees()
    {
        return m_reqFEmployees;
    }

    QShiftDay::QShiftDay()
    {
        m_bManagers.clear();
        m_bfManagers.clear();
        m_fEmployees.clear();
        m_date = QDateTime::currentDateTime();
    }

    QShiftDay::QShiftDay(const QDateTime &dt, EmployeeMap &bm, EmployeeMap &bfm, EmployeeMap &fe) :
        m_bManagers(bm), m_bfManagers(bfm), m_fEmployees(fe)
    {
        m_date = dt;
        if (m_date.date().dayOfWeek() == 7) {
            m_reqFEmployees = 6;
        } else
        {
            m_reqFEmployees = 9;
        }
        m_reqManagers = 6;
        m_reqFManagers = 4;
    }

    QShiftDay::~QShiftDay()
    {
    }

    /**
     * @brief QShiftSolver::QShiftSolver
     * @param parent
     */
    QShiftSolver::QShiftSolver(QObject *parent)
        : QObject(parent)
    {
    }

    QShiftSolver::QShiftSolver(EmployeeGroup &man, EmployeeGroup &fman, EmployeeGroup &fe, QObject *parent):
        m_manGroup(man),
        m_fuelManGroup(fman),
        m_fuelEmployeeGroup(fe),
        QObject(parent)
    {

    }

    QShiftSolver::~QShiftSolver()
    {
    }

    void QShiftSolver::setEmployeeGroups(EmployeeGroup &man, EmployeeGroup &fman, EmployeeGroup &fe)
    {
        m_manGroup = man;
        m_fuelManGroup = fman;
        m_fuelEmployeeGroup = fe;
    }

    int QShiftSolver::solve(EmployeeGroup &eg)
    {
        //TODO : Enter you code bellow
        return 0;
    }

    Shifts &QShiftSolver::initShifts()
    {
        //TODO : Enter your code bellow!
        return m_pShifts;
    }

    Shifts &QShiftSolver::shifts()
    {
        return m_pShifts;
    }

    int QShiftSolver::solve_managers()
    {

    }
}
