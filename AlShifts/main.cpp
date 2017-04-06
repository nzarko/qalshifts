#include "mainwindow.h"
#include "manhattanstyle.h"

#include <QApplication>
#include <QDebug>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //The bellow info are being used with the default QSettings constructor.
    QCoreApplication::setOrganizationName("Algorithmos");
    QCoreApplication::setOrganizationDomain("algorithmos.com");
    QCoreApplication::setApplicationName("qalshifts");

    QTranslator translator;
    // look up e.g. :/translations/myapp_de.qm
    if (translator.load(QLocale(), QLatin1String("qalshifts"), QLatin1String("_"), QLatin1String(":/translations")))
        a.installTranslator(&translator);
    else
        qDebug() << "Could load translation file" << endl;


    QString basename = a.style()->objectName();
    a.setStyle(new ManhattanStyle(basename));

    MainWindow *w = MainWindow::instance();
    w->show();

    return a.exec();
}
