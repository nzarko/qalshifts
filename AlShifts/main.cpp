#include "mainwindow.h"
#include "manhattanstyle.h"

#include <QApplication>
#include <QDebug>
#include <QLocale>
#include <QTranslator>
#include <QTextCodec>
#include <QLibraryInfo>

int main(int argc, char *argv[])
{
//    QLocale el_locale(QLocale::Greek, QLocale::Greece);
//    QLocale::setDefault(el_locale);

    QCoreApplication::addLibraryPath(".");
    QApplication a(argc, argv);
    //The bellow info are being used with the default QSettings constructor.
    QCoreApplication::setOrganizationName("Algorithmos");
    QCoreApplication::setOrganizationDomain("algorithmos.com");
    QCoreApplication::setApplicationName("qalshifts");

    QTranslator qtTranslator;
    if (qtTranslator.load(QLocale(),QLatin1String("qt"), QLatin1String("_"),
                          QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
        qDebug() << "Qt translation dir : " << QLibraryInfo::location(QLibraryInfo::TranslationsPath) << endl;
        a.installTranslator(&qtTranslator);
    } else
        qDebug() << "Qt translations dir not found." << endl;

    QTranslator translator;
    // look up e.g. :/translations/myapp_de.qm
    if (translator.load(QLocale(), QLatin1String("qalshifts"), QLatin1String("_"), QLatin1String(":/translations")))
        a.installTranslator(&translator);
    else
        qDebug() << "Could load translation file" << endl;

    QTranslator sc_translator;
    // look up e.g. :/translations/myapp_de.qm
    if (sc_translator.load(QLocale(), QLatin1String("qshiftscore"), QLatin1String("_"), QLatin1String(":/translations")))
        a.installTranslator(&sc_translator);
    else
        qDebug() << "Could load translation file" << endl;

    QString basename = a.style()->objectName();
    a.setStyle(new ManhattanStyle(basename));

    MainWindow *w = MainWindow::instance();
    w->show();

    return a.exec();
}
