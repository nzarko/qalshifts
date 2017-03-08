#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <functional>
#include <iterator>
#include <numeric>
#include <set>
#include <stdio.h>

#include <QDebug>
#include <QFile>
#include <QTextStream>


#include "qshiftsolver.h"




namespace Algorithmos {

StringList QShiftSolver::m_reqBranches = StringList();

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
        m_bManagers.clear();
        m_bfManagers.clear();
        m_fEmployees.clear();
        if (m_date.date().dayOfWeek() == 7) {
            m_reqFEmployees = 6;
        } else
        {
            m_reqFEmployees = 9;
        }
        m_reqManagers = 6;
        m_reqFManagers = 4;
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


    typedef std::vector<std::string> StringList;
    typedef std::vector<std::vector<std::string> > StringListArray;
    /**
     * @brief QShiftSolver::QShiftSolver
     * @param parent
     */
    QShiftSolver::QShiftSolver(QObject *parent)
        : QObject(parent),
          m_smatrix(7,49),
          fm_smatrix(7,49),
          ef_smatrix(8,49)
    {
        init_matrix_with_zeros(m_smatrix);
        init_matrix_with_zeros(fm_smatrix);
        init_matrix_with_zeros(ef_smatrix);
    }

    QShiftSolver::QShiftSolver(EmployeeGroup &man, EmployeeGroup &fman, EmployeeGroup &fe, QObject *parent):
        m_manGroup(man),
        m_fuelManGroup(fman),
        m_fuelEmployeeGroup(fe),
        QObject(parent),
        m_smatrix(7,49),
        fm_smatrix(7,49),
        ef_smatrix(8,49)
    {
        qDebug() << "From QShiftSolver ctor :\n"
                 << "Managers contains : " << m_manGroup.size() << " managers!\n"
                 <<"Fuel Managers Contains : " << m_fuelManGroup.size() << " fuel managers!\n"
                 <<"Employees Contains : " << m_fuelEmployeeGroup.size() << " fuel employees!" << endl;
        init_matrix_with_zeros(m_smatrix);
        init_matrix_with_zeros(fm_smatrix);
        init_matrix_with_zeros(ef_smatrix);
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
        Q_UNUSED(eg)
        //TODO : Enter you code bellow
        return 0;
    }

    Shifts &QShiftSolver::initShifts(QDateTime dt)
    {
        //Initialize shifts.
        //We'll start from Managers (more easy than the others).
        m_currentShift = new QShiftDay(dt);
        QDateTime shift_date = m_currentShift->shiftDate();

        ///TODO : implement matrix solution.
        m_smatrix = create_managers_shifts_matrix();
        fm_smatrix = create_fuel_managers_shifts_matrix();
        //fm_smatrix = m_smatrix;
        //qDebug() << m_smatrix << endl;

        //Create the employees matrix
        ef_smatrix = create_employees_shifts_matrix();
        //qDebug() << ef_smatrix << endl;

        //Fill the Shifts vector compining names from
        //m_manGroup and shifts from m_smatrix
        QVector<QEmployee *> early_vec,f_early_vec,e_early_vec;
        QVector<QEmployee *> late_vec,f_late_vec,e_late_vec;
        QVector<QEmployee *> dayof_vec,f_dayoff_vec,e_dayoff_vec;
        QVector<QEmployee *> e_intermittent_vec;
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

            // For branch fuel employees

            for(size_t i = 0; i < fm_smatrix.size1(); i++) {
                Algorithmos::ShiftType type = (Algorithmos::ShiftType)fm_smatrix(i,j);
                switch (type) {
                case Algorithmos::EARLY:
                    f_early_vec.push_back(m_fuelManGroup[i]);
                    break;
                case Algorithmos::LATE:
                    f_late_vec.push_back(m_fuelManGroup[i]);
                    break;
                case Algorithmos::DAYOFF:
                    f_dayoff_vec.push_back(m_fuelManGroup[i]);
                    break;
                case Algorithmos::INTERMITTENT:
                    break;
                }
            }
            //For employees shift matrix need to take care in seperate loop.
            for(size_t i = 0; i < ef_smatrix.size1(); i++) {
                Algorithmos::ShiftType type = (Algorithmos::ShiftType)ef_smatrix(i,j);
                switch (type) {
                case Algorithmos::EARLY:
                    e_early_vec.push_back(m_fuelEmployeeGroup[i]);
                    break;
                case Algorithmos::LATE:
                    e_late_vec.push_back(m_fuelEmployeeGroup[i]);
                    break;
                case Algorithmos::DAYOFF:
                    e_dayoff_vec.push_back(m_fuelEmployeeGroup[i]);
                    break;
                case Algorithmos::INTERMITTENT:
                    e_intermittent_vec.push_back(m_fuelEmployeeGroup[i]);
                    break;
                }
            }
            m_currentShift->bManagers().insert(Algorithmos::EARLY,early_vec);
            m_currentShift->bManagers().insert(Algorithmos::LATE, late_vec);
            m_currentShift->bManagers().insert(Algorithmos::DAYOFF, dayof_vec);
            m_currentShift->bfManagers().insert(Algorithmos::EARLY,f_early_vec);
            m_currentShift->bfManagers().insert(Algorithmos::LATE, f_late_vec);
            m_currentShift->bfManagers().insert(Algorithmos::DAYOFF, f_dayoff_vec);
            m_currentShift->fEmployees().insert(Algorithmos::EARLY,e_early_vec);
            m_currentShift->fEmployees().insert(Algorithmos::LATE, e_late_vec);
            m_currentShift->fEmployees().insert(Algorithmos::INTERMITTENT, e_intermittent_vec);
            m_currentShift->fEmployees().insert(Algorithmos::DAYOFF, e_dayoff_vec);
            m_pShifts.push_back(m_currentShift);

            shift_date = shift_date.addDays(1);
            m_currentShift = new QShiftDay(shift_date);

            early_vec.clear(); f_early_vec.clear(); e_early_vec.clear();
            late_vec.clear(); f_late_vec.clear(); e_late_vec.clear();
            dayof_vec.clear(); f_dayoff_vec.clear(); e_dayoff_vec.clear();
            e_intermittent_vec.clear();
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
        m_smatrix(5,0) = 1;
        m_smatrix(6,0) = 1;
        //Last Column
        m_smatrix(1,48) = 1;
        m_smatrix(3,48) = 1;
        m_smatrix(5,48) = 1;
        //Rest of columns
        for(uint j = 1; j < m_smatrix.size2() - 1; j++) {
            for(uint i = 0; i < m_smatrix.size1(); i++) {
                if(m_smatrix(i,j) == 3 || m_smatrix(i,j-1) == 3 || m_smatrix(i,j+1) == 3)
                    continue;
                m_smatrix(i,j) = (m_smatrix(i,j-1)+ 1) % 2;
            }
        }

        //Special treatment for "naked days" :
        //days without dayoff shift.
        //Starts from column 7
        int r = 1;
        for(uint j = 7; j < m_smatrix.size2(); j += 8) {
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

    UBlas::matrix<int> &QShiftSolver::create_fuel_managers_shifts_matrix()
    {
        /*size_t i,j=0;
        fm_smatrix(0,1) = 1;
        for(j = 0; j < fm_smatrix.size2(); j+=8)
            for(i=0; i<fm_smatrix.size1();i++) {
                if ( i + j < fm_smatrix.size2()) {
                    fm_smatrix(i,i+j) = 3;
                    //neighours must have specific values
                    if (i + j> 0 && j + i + 1 < fm_smatrix.size2()) {
                        fm_smatrix(i,i+j-1) = 0;
                        fm_smatrix(i, i + j + 1) = 1;
                    }
                }
            }

        //First column
        m_smatrix(2,0) = 1;
        m_smatrix(5,0) = 1;
        m_smatrix(6,0) = 1;
        //Last Column
        m_smatrix(1,48) = 1;
        m_smatrix(3,48) = 1;
        m_smatrix(5,48) = 1;

        //Rest of columns
        for(uint j = 1; j < fm_smatrix.size2() - 1; j++) {
            for(uint i = 0; i < fm_smatrix.size1(); i++) {
                if(fm_smatrix(i,j) == 3 || fm_smatrix(i,j-1) == 3 || fm_smatrix(i,j+1) == 3)
                    continue;
                fm_smatrix(i,j) = (fm_smatrix(i,j-1)+ 1) % 2;
            }
        }

        //finishing out
        for(size_t j = 1; j < fm_smatrix.size2(); j++) {
            if((count_col_zeros(fm_smatrix,j)) == 4) {
                int r = find003(fm_smatrix,j);
                if (r != -1)
                    fm_smatrix(r,j) = 1;
            }
        }
        //Check for Employees in rows 2 and 4. They must not have the same shift in the same day
        for(size_t j = 1; j < ef_smatrix.size2()- 1; j++) {
            if (fm_smatrix(2,j) == fm_smatrix(4,j)) {
                if(fm_smatrix(2,j-1) == 3 || fm_smatrix(2,j+1) == 3)
                    fm_smatrix(4,j) = (fm_smatrix(4,j) + 1) % 2;
                else fm_smatrix(2,j) = (fm_smatrix(2,j) + 1) % 2;
                fm_smatrix(3,j) = (fm_smatrix(3,j) + 1) % 2;
            }
            //configure the remaining items

        }*/

        fm_smatrix = m_smatrix;
        //Check for Employees in rows 2 and 4. They must not have the same shift in the same day
        for(size_t j = 1; j < ef_smatrix.size2()- 1; j++) {
            if (fm_smatrix(2,j) == fm_smatrix(4,j)) {
                if(fm_smatrix(2,j-1) == 3 || fm_smatrix(2,j+1) == 3)
                    fm_smatrix(4,j) = (fm_smatrix(4,j) + 1) % 2;
                else fm_smatrix(2,j) = (fm_smatrix(2,j) + 1) % 2;
                if(fm_smatrix(3,j) !=3)
                    fm_smatrix(3,j) = (fm_smatrix(3,j) + 1) % 2;
                else
                    fm_smatrix(5,j) = (fm_smatrix(5,j) + 1) % 2;
            }
            //configure the remaining items

        }
        //Last column :
        fm_smatrix(1,48) = 1;
        fm_smatrix(2,48) = fm_smatrix(3,48) = 0;
        fm_smatrix(4,48) = fm_smatrix(5,48) = 1;
        fm_smatrix(6,48) = 0;
        return fm_smatrix;
    }

    UBlas::matrix<int> &QShiftSolver::managersShiftsMatrix()
    {
        return m_smatrix;
    }

    UBlas::matrix<int> &QShiftSolver::fuelmanagersShiftsMatrix()
    {
        return fm_smatrix;
    }

    UBlas::matrix<int> &QShiftSolver::create_employees_shifts_matrix()
    {
        ///TODO : Implement me!!!
        ///
       /* size_t i,j=0;
        ef_smatrix(0,1) = 1;
        for(j = 0; j < ef_smatrix.size2(); j+=8)
            for(i=0; i<ef_smatrix.size1();i++) {
                if ( i + j < ef_smatrix.size2()) {
                    ef_smatrix(i,i+j) = 3;
                    //neighours must have specific values
                    if (i + j> 0 && j + i + 1 < ef_smatrix.size2()) {
                        ef_smatrix(i,i+j-1) = 0;
                        ef_smatrix(i, i + j + 1) = 1;
                    }
                }
            }
        //First column
        ef_smatrix(1,0) = 2;
        ef_smatrix(2,0) = 1;
        ef_smatrix(4,0) = 2;
        ef_smatrix(6,0) = 2;
        //Last Column
        ef_smatrix(1,48) = 1;
        ef_smatrix(3,48) = 1;
        ef_smatrix(5,48) = 1;
        //Rest of columns
        for(uint j = 1; j < ef_smatrix.size2() - 1; j++) {
            for(uint i = 0; i < ef_smatrix.size1(); i++) {
                if(ef_smatrix(i,j) == 3 || ef_smatrix(i,j-1) == 3 || ef_smatrix(i,j+1) == 3)
                    continue;
                ef_smatrix(i,j) = (ef_smatrix(i,j-1)+ 1) % 3;
            }
        }

        //finishing out
        for(size_t j = 1; j < ef_smatrix.size2(); j++) {
            if(count_col_zeros(ef_smatrix,j) == 4) {
                int r = find003(ef_smatrix,j);
                if (r != -1 && r != 5)
                    ef_smatrix(r,j) = 1;
            }
        }*/

        /* *************************************** *
         * Read the ef_smatrix contents from file
         * *************************************** */
        QFile f("fe_matrix.txt");
        if(!f.open(QIODevice::ReadOnly))
            qDebug() << " Error while trying to read " << f.fileName() << " file. " << endl;

        QTextStream ts(&f);
        QString line;
        int num;
        ///TODO Finish me!!
        return ef_smatrix;
    }

    UBlas::matrix<int> &QShiftSolver::employeeShiftsMatrix()
    {
        return ef_smatrix;
    }

    QStringList QShiftSolver::solve_branch_shifts(StringListArray sla)
    {        
        StringListArray combs = find_compinations_of(sla);
        QStringList res;
        int req_len = combs[0].size();
        std::set<std::string> checker;
        if(m_reqBranches.empty()) {
            for (auto x : combs) {
                checker.insert(x.begin(), x.end());
                if (checker.size() == req_len )
                {
                    return sl_to_qsl(x);
                }
                checker.clear();
            }
        } else {
            for (auto x : combs) {
                checker.insert(x.begin(), x.end());
                if ((checker.size() == req_len) )
                {
                    int cnt = 0;
                    for(auto rb : m_reqBranches) {
                        auto pos = std::find(std::begin(checker),std::end(checker), rb);
                        if(pos != std::end(checker))
                            cnt++;
                    }
                    if(cnt == m_reqBranches.size())
                        return sl_to_qsl(x);
                }
                checker.clear();
            }
        }
        clear_req_branches();
        return res;
    }

    void QShiftSolver::set_required_branches(StringList sl)
    {
        m_reqBranches = sl;
    }

    void QShiftSolver::clear_req_branches()
    {
        m_reqBranches.clear();
    }

    void QShiftSolver::init_matrix_with_zeros(UBlas::matrix<int> &mat)
    {
        for(uint i = 0; i < mat.size1(); i++) {
            for(uint j = 0; j < mat.size2(); j++) {
                mat(i,j) = 0;
            }
        }
    }

    int QShiftSolver::solve_managers()
    {
        return 0;
    }

    int QShiftSolver::count_col_zeros(const UBlas::matrix<int> &mat, size_t j)
    {
        if (j < mat.size2()) {
            int count = 0;
            for(uint i=0; i< mat.size1(); i++) {
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
                if ((mat(i,j) == 0) && (mat(i,j+1)==0) & (mat(i,j+2) == 3))
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
