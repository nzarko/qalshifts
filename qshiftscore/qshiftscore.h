#ifndef QSHIFTSCORE_H
#define QSHIFTSCORE_H

#include "qshiftsolver.h"
namespace Algorithmos {
class QSHIFTSCORE_EXPORT QShiftsCore
{

public:
    QShiftsCore();
    ~QShiftsCore();
    void init();
    QShiftSolver *solver();
    QVector<QEmployee* > branchManagers();
    QVector<QEmployee* > branchFuelManagers();
    QVector<QEmployee* > fuelEmployees();
private:
    QShiftSolver *m_solver;
    QVector<QEmployee* > bManagers;
    QVector<QEmployee* > bfManagers;
    QVector<QEmployee* > bEmployees;

};
}
#endif // QSHIFTSCORE_H
