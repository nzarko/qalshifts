#include "mainwindow.h"
#include "qshiftscore.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    Algorithmos::QShiftsCore s_core;
    s_core.init();
    return a.exec();
}
