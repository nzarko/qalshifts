#-------------------------------------------------
#
# Project created by QtCreator 2017-01-26T00:01:36
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AlShifts
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

DEFINES += _ATL_XP_TARGETING
DEFINES += PSAPI_VERSION=1
QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../qshiftscore/release/ -lqshiftscore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../qshiftscore/debug/ -lqshiftscore
else:unix: LIBS += -L$$OUT_PWD/../qshiftscore/ -lqshiftscore

INCLUDEPATH += $$PWD/../qshiftscore
DEPENDPATH += $$PWD/../qshiftscore