#include "mainwindow.h"
#include "manhattanstyle.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //The bellow info are being used with the default QSettings constructor.
    QCoreApplication::setOrganizationName("Algorithmos");
    QCoreApplication::setOrganizationDomain("online-edu.com");
    QCoreApplication::setApplicationName("Pascal-Dev");

    QString basename = a.style()->objectName();
    a.setStyle(new ManhattanStyle(basename));

    MainWindow *w = MainWindow::instance();
    w->show();

    return a.exec();
}
