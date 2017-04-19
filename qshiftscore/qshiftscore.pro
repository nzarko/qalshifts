#-------------------------------------------------
#
# Project created by QtCreator 2017-01-25T17:05:28
#
#-------------------------------------------------

QT       += widgets

QT       -= gui

TARGET = qshiftscore
TEMPLATE = lib

DEFINES += QSHIFTSCORE_LIBRARY
win32 {
DEFINES += _ATL_XP_TARGETING
DEFINES += PSAPI_VERSION=1
QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01
QMAKE_CFLAGS += /D _USING_V110_SDK71_
QMAKE_CXXFLAGS += /D _USING_V110_SDK71_
LIBS *= -L"%ProgramFiles(x86)%/Microsoft SDKs/Windows/7.1A/Lib"
INCLUDEPATH += "%ProgramFiles(x86)%/Microsoft SDKs/Windows/7.1A/Include"
}

#win32 {
copydata.commands = $(COPY_DIR) $$shell_path($$PWD/../Data)  $$shell_path($$OUT_PWD/../Data)
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
QMAKE_EXTRA_TARGETS += first copydata
#}
# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += qshiftscore.cpp \
    qconstraint.cpp \
    qemployee.cpp \
    utils.cpp \
    qshiftsolver.cpp

HEADERS += qshiftscore.h\
        qshiftscore_global.h \
    qconstraint.h \
    qemployee.h \
    utils.h \
    qshiftsolver.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32: INCLUDEPATH += C:/local/boost_1_62_0

TRANSLATIONS += translations/qshiftscore_el.ts
