#include "qshiftsolver.h"

#include <QDebug>
#include <QQueue>

namespace Algorithmos {

/**
     * @brief The Available struct
     * Used for the availibility of branch fuels managers
     * These managers fill the empty shifts position from
     * fuel employees.
     */
    struct Available {
        QEmployee *e1;
        QEmployee *e2;
    };

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
        if (m_date.date().dayOfWeek() == 7) {
            m_reqFEmployees = 6;
        } else
        {
            m_reqFEmployees = 9;
        }
        m_reqManagers = 6;
        m_reqFManagers = 4;
    }

    QShiftDay::QShiftDay(const QDateTime &dt)
    {
        m_date = dt;
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
        qDebug() << "From QShiftSolver ctor :\n"
                 << "Managers contains : " << m_manGroup.size() << " managers!\n"
                 <<"Fuel Managers Contains : " << m_fuelManGroup.size() << " fuel managers!\n"
                 <<"Employees Contains : " << m_fuelEmployeeGroup.size() << " fuel employees!" << endl;
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
        //Initialize shifts.
        //We'll start from Managers (more easy than the others).
        m_currentShift = new QShiftDay();
        QDateTime shift_date = m_currentShift->shiftDate();
        /* Fill three queues with necessary employees from m_manGroup.
         * 1. earlyQueue : contains employees for early shift
         * 2. lateQueue  : contains employees for late shift
         * 3 dayoffQueue : contains employees for day off.
         */
        //TODO : Enter your code bellow!
        QQueue<QEmployee*> man_early_queue;
        QQueue<QEmployee *> man_late_queue;
        QQueue<QEmployee*> man_dayoff_queue;
        for(int i = 0; i < m_currentShift->reqManagers() / 2; i++)
            man_early_queue.enqueue(m_manGroup[i]);
        for(int i = m_currentShift->reqManagers()/2; i < m_currentShift->reqManagers(); i++)
            man_late_queue.enqueue(m_manGroup[i]);
        man_dayoff_queue.enqueue(m_manGroup.last());

        m_currentShift->bManagers().insert(Algorithmos::EARLY,QVector<QEmployee*>::fromList(man_early_queue));
        m_currentShift->bManagers().insert(Algorithmos::LATE, QVector<QEmployee*>::fromList(man_late_queue));
        m_currentShift->bManagers().insert(Algorithmos::DAYOFF, QVector<QEmployee*>::fromList(man_dayoff_queue));
        qDebug() << "Shift Day Managers  for : "
                 << shift_date << " : "
                 <<m_currentShift->bManagers() << endl;
        m_pShifts.push_back(m_currentShift);

//        qDebug() << "Queues at 1st round : \n"
//                 << man_early_queue << endl
//                 << man_late_queue << endl
//                 << man_dayoff_queue <<endl;
        int req_shift_man = m_currentShift->reqManagers() /2 ;
        //try it for the rest 48 days !!!
        for(int j = 2; j <= 49; j++) {
            //first create date:
            shift_date = shift_date.addDays(1);
            m_currentShift = new QShiftDay(shift_date);
            /*
         * Four moves :
         * 1) take the first element from early_queue and push it to dayoff_queue
         * 2) take the first element from dayoff_queue and push to late_queue
         * 3) take the three first elements from late_queue and push them to early_queue
         * 4) take the two first elements from early_queue and push them to late_queue
         */
            int i;
            // 1)
            man_dayoff_queue.enqueue(man_early_queue.dequeue());
            // 2)
            man_late_queue.enqueue(man_dayoff_queue.dequeue());
            // 3)
            for(i = 1; i <= req_shift_man; i++)
                man_early_queue.enqueue(man_late_queue.dequeue());
            // 4)
            for(i = 1; i <= req_shift_man-1; i++)
                man_late_queue.enqueue(man_early_queue.dequeue());

            m_currentShift->bManagers().insert(Algorithmos::EARLY,QVector<QEmployee*>::fromList(man_early_queue));
            m_currentShift->bManagers().insert(Algorithmos::LATE, QVector<QEmployee*>::fromList(man_late_queue));
            m_currentShift->bManagers().insert(Algorithmos::DAYOFF, QVector<QEmployee*>::fromList(man_dayoff_queue));
            qDebug() << "Shift Day Managers  for : "
                     << shift_date << " : "
                     <<m_currentShift->bManagers() << endl;
            m_pShifts.push_back(m_currentShift);

//            qDebug() << "Queues at " << j+1 << "nd round : \n"
//                     << man_early_queue << endl
//                     << man_late_queue << endl
//                     << man_dayoff_queue <<endl;
        }

        /*
         * Now its time for others. Be careful !!
         * Now we don't push_back values but we gone use
         * the [] operator for m_pShifts.
         */
        ///TODO : Add you code for other type of employees.
        // Use the above queues for the fuel managers
        //First clear the lists
        man_dayoff_queue.clear();
        man_early_queue.clear();
        man_late_queue.clear();
        QVector<Available *> availables;
        QQueue<QEmployee *> fuel_early_queue;
        QQueue<QEmployee *> fuel_late_queue;
        QQueue<QEmployee *> fuel_intermittent_queue;
        QQueue<QEmployee *> fuel_dayoff_queue;
        //First round. Init.
        m_currentShift = m_pShifts[0];
        shift_date = m_currentShift->shiftDate();
        int empl_needed = m_currentShift->reqFManagers() / 2;
        int cur_i;
        for( cur_i = 0; cur_i < empl_needed; cur_i++ )
            man_early_queue.enqueue(m_fuelManGroup[cur_i]);
        for( cur_i = empl_needed; cur_i < empl_needed + 2; cur_i++)
            man_late_queue.enqueue(m_fuelManGroup[cur_i]);
        man_dayoff_queue.enqueue(m_fuelManGroup[cur_i]);
        qDebug() << "Branch Fuel Managers, round 1: "
                 << man_early_queue
                 << man_late_queue
                 << man_dayoff_queue
                 << endl;
        if ( cur_i + 2 < m_fuelManGroup.size()){
            Available av = { m_fuelManGroup[cur_i+1], m_fuelManGroup[cur_i+2] };
            availables.push_back(&av);
            qDebug() << "Availables : " << av.e1 << " , " <<av.e2 <<  endl;
        }

        //Initialize fuel employees.



        return m_pShifts;
    }

    Shifts &QShiftSolver::shifts()
    {
        return m_pShifts;
    }

    int QShiftSolver::solve_managers()
    {
        return 0;
    }
}
