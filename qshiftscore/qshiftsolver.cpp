#include <iostream>
#include <algorithm>
#include <fstream>

#include <QDebug>


#include "qshiftsolver.h"




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
        : QObject(parent),
          m_smatrix(7,49)
    {
        for(size_t i = 0; i < m_smatrix.size1(); i++)
            for(size_t j = 0; j < m_smatrix.size2(); j++)
                m_smatrix(i,j) = 0;
    }

    QShiftSolver::QShiftSolver(EmployeeGroup &man, EmployeeGroup &fman, EmployeeGroup &fe, QObject *parent):
        m_manGroup(man),
        m_fuelManGroup(fman),
        m_fuelEmployeeGroup(fe),
        QObject(parent),
        m_smatrix(7,49)
    {
        qDebug() << "From QShiftSolver ctor :\n"
                 << "Managers contains : " << m_manGroup.size() << " managers!\n"
                 <<"Fuel Managers Contains : " << m_fuelManGroup.size() << " fuel managers!\n"
                 <<"Employees Contains : " << m_fuelEmployeeGroup.size() << " fuel employees!" << endl;
        for(size_t i = 0; i < m_smatrix.size1(); i++)
            for(size_t j = 0; j < m_smatrix.size2(); j++)
                m_smatrix(i,j) = 0;
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

        ///TODO : implement matrix solution.
        m_smatrix = create_managers_shifts_matrix();
        qDebug() << m_smatrix << endl;

        //Fill the Shifts vector compining names from
        //m_manGroup and shifts from m_smatrix
        QVector<QEmployee *> early_vec;
        QVector<QEmployee *> late_vec;
        QVector<QEmployee *> dayof_vec;
        for(size_t j = 0; j < m_smatrix.size2(); j++) {
            for(size_t i = 0; i < m_smatrix.size1(); i++) {
                Algorithmos::ShiftType type = (Algorithmos::ShiftType)m_smatrix(i,j);
                switch (type) {
                case Algorithmos::EARLY:
                    early_vec.push_back(m_manGroup[i]);
                    break;
                case Algorithmos::LATE:
                    late_vec.push_back(m_manGroup[i]);
                    break;
                case Algorithmos::DAYOFF:
                    dayof_vec.push_back(m_manGroup[i]);
                    break;
                case Algorithmos::INTERMITTENT:
                    break;
                }
            }
            m_currentShift->bManagers().insert(Algorithmos::EARLY,early_vec);
            m_currentShift->bManagers().insert(Algorithmos::LATE, late_vec);
            m_currentShift->bManagers().insert(Algorithmos::DAYOFF, dayof_vec);
            m_pShifts.push_back(m_currentShift);

            shift_date = shift_date.addDays(1);
            m_currentShift = new QShiftDay(shift_date);

            early_vec.clear();
            late_vec.clear();
            dayof_vec.clear();
        }

        return m_pShifts;
    }

    Shifts &QShiftSolver::shifts()
    {
        return m_pShifts;
    }

    UBlas::matrix<int> &QShiftSolver::create_managers_shifts_matrix()
    {
        size_t i,j=0;
        m_smatrix(0,1) = 1;
        for(j = 0; j < m_smatrix.size2(); j+=8)
            for(i=0; i<m_smatrix.size1();i++) {
                if ( i + j < m_smatrix.size2()) {
                    m_smatrix(i,i+j) = 3;
                    //neighours must have specific values
                    if (i + j> 0 && j + i + 1 < m_smatrix.size2()) {
                        m_smatrix(i,i+j-1) = 0;
                        m_smatrix(i, i + j + 1) = 1;
                    }
                }
            }
        //First column
        m_smatrix(2,0) = 1;
        m_smatrix(4,0) = 1;
        m_smatrix(6,0) = 1;
        //Last Column
        m_smatrix(1,48) = 1;
        m_smatrix(3,48) = 1;
        m_smatrix(5,48) = 1;
        //Rest of columns
        for(int j = 1; j < m_smatrix.size2() - 1; j++) {
            for(int i = 0; i < m_smatrix.size1(); i++) {
                if(m_smatrix(i,j) == 3 || m_smatrix(i,j-1) == 3 || m_smatrix(i,j+1) == 3)
                    continue;
                m_smatrix(i,j) = (m_smatrix(i,j-1)+ 1) % 2;
            }
        }

        //Special treatment for "naked days" :
        //days without dayoff shift.
        //Starts from column 7
        int r = 1;
        for(int j = 7; j < m_smatrix.size2(); j += 8) {
            m_smatrix(r,j) = 3;
            r++;
        }

        //finishing out
        for(size_t j = 1; j < m_smatrix.size2(); j++) {
            if(count_col_zeros(m_smatrix,j) == 4) {
                int r = find003(m_smatrix,j);
                if (r != -1)
                    m_smatrix(r,j) = 1;
            }
        }
        return m_smatrix;
    }

    UBlas::matrix<int> &QShiftSolver::managersShiftsMatrix()
    {
        return m_smatrix;
    }

    int QShiftSolver::solve_managers()
    {
        return 0;
    }

    int QShiftSolver::count_col_zeros(const UBlas::matrix<int> &mat, size_t j)
    {
        if (j < mat.size2()) {
            int count = 0;
            for(int i=0; i< mat.size1(); i++) {
                if ( mat(i,j) == 0)
                    count ++;
            }
            return count;
        } else {
            qDebug() << "Out of bounds!" << endl;
            return -1 ;
        }
    }

    int QShiftSolver::find003(const UBlas::matrix<int> &mat, size_t j)
    {
        int res = -1;
        if ( j + 2 < mat.size2()) {
            for(size_t i = 0; i < mat.size1(); i++) {
                if (mat(i,j) == 0 && mat(i,j+1)==0 & mat(i,j+2) == 3)
                    res = i;
            }
        } else if (j + 1 == mat.size2() - 1) {
            return 5;
        }
        return res;
    }

    QDebug operator<<(QDebug debug, const UBlas::matrix<int> &mat)
    {
        QDebugStateSaver saver(debug);
        for(size_t i = 0; i < mat.size1(); i++) {
            for(size_t j = 0; j < mat.size2(); j++) {
                debug.nospace() << mat(i,j) << "   ";
            }
            debug.nospace() << '\n';
        }

        return debug;
    }

}
