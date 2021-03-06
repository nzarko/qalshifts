#-------------------------------------------------
#
# Project created by QtCreator 2017-01-26T00:01:36
#
#-------------------------------------------------

QT       += core gui printsupport webenginewidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AlShifts
TEMPLATE = app
unix:CONFIG += c++11

win32 {
QMAKE_CXXFLAGS += /utf-8
}
# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


include(xlsx/qtxlsx.pri)

SOURCES += main.cpp\
        mainwindow.cpp \
    centralview.cpp \
    qemployeeshiftstable.cpp \
    qshiftstableitem.cpp \
    legendform.cpp \
    qshifttableitemdelegate.cpp \
    qdateselector.cpp \
    headerview.cpp \
    headerdelegate.cpp \
    qemployeeshiftsweeklyreport.cpp \
    weekreportinfo.cpp \
    printview.cpp \
    fancylineedit.cpp \
    historycompleter.cpp \
    manhattanstyle.cpp \
    qtcassert.cpp \
    styleanimator.cpp \
    stylehelper.cpp \
    configuration.cpp \
    alshiftssettingsdialog.cpp \
    employeeeditorform.cpp \
    alshiftstreewidget.cpp \
    qabstractsettingswidget.cpp \
    qstaffweeklyreport.cpp \
    aboutdialog.cpp \
    stuffprintview.cpp \
    abstractprintview.cpp \
    managersdoreportdialog.cpp

HEADERS  += mainwindow.h \
    centralview.h \
    qemployeeshiftstable.h \
    qshiftstableitem.h \
    legendform.h \
    qshifttableitemdelegate.h \
    qdateselector.h \
    headerview.h \
    headerdelegate.h \
    qemployeeshiftsweeklyreport.h \
    weekreportinfo.h \
    printview.h \
    fancylineedit.h \
    historycompleter.h \
    manhattanstyle.h \
    qtcassert.h \
    styleanimator.h \
    stylehelper.h \
    configuration.h \
    alshiftssettingsdialog.h \
    employeeeditorform.h \
    alshiftstreewidget.h \
    qabstractsettingswidget.h \
    qstaffweeklyreport.h \
    aboutdialog.h \
    stuffprintview.h \
    abstractprintview.h \
    managersdoreportdialog.h

FORMS    += mainwindow.ui \
    legendform.ui \
    qemployeeshiftsweeklyreport.ui \
    alshiftssettingsdialog.ui \
    employeeeditorform.ui \
    aboutdialog.ui \
    managersdoreportdialog.ui
win32 {
DEFINES += _ATL_XP_TARGETING
DEFINES += PSAPI_VERSION=1
QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01
QMAKE_CFLAGS += /D _USING_V110_SDK71_
QMAKE_CXXFLAGS += /D _USING_V110_SDK71_
LIBS *= -L"%ProgramFiles(x86)%/Microsoft SDKs/Windows/7.1A/Lib"
INCLUDEPATH += "%ProgramFiles(x86)%/Microsoft SDKs/Windows/7.1A/Include"
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../qshiftscore/release/ -lqshiftscore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../qshiftscore/debug/ -lqshiftscore
else:unix: LIBS += -L$$OUT_PWD/../qshiftscore/ -lqshiftscore
win32: INCLUDEPATH += C:/local/boost_1_62_0

INCLUDEPATH += $$PWD/../qshiftscore
DEPENDPATH += $$PWD/../qshiftscore

RESOURCES += \
    resources.qrc

TRANSLATIONS += translations/AlShifts_el.ts
