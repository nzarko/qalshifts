#include "mainwindow.h"
#include "qshiftscore.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    Algorithmos::QShiftsCore s_core;
    Algorithmos::QShiftSolver *s_solver = s_core.solver();
    if(s_solver)
        s_solver->initShifts();
    else
        qDebug() << "Solver could not initialized. Check QShiftSolver::init()" << endl;


    return a.exec();
}
