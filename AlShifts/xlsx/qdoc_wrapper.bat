@echo off
SetLocal EnableDelayedExpansion
(set QT_VERSION=0.3.0)
(set QT_VER=0.3)
(set QT_VERSION_TAG=030)
(set QT_INSTALL_DOCS=C:/Qt/Docs/Qt-5.8)
(set BUILDDIR=G:/src/qt_prjs/QtXlsxWriter/src/xlsx)
C:\Qt\5.8\msvc2015\bin\qdoc.exe %*
EndLocal
