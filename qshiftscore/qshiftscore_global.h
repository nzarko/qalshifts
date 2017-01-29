#ifndef QSHIFTSCORE_GLOBAL_H
#define QSHIFTSCORE_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QString>
#include <QVector>
#include <QMap>
#include <QStringList>

#if defined(QSHIFTSCORE_LIBRARY)
#  define QSHIFTSCORE_EXPORT Q_DECL_EXPORT
#else
#  define QSHIFTSCORE_EXPORT Q_DECL_IMPORT
#endif

#endif // QSHIFTSCORE_GLOBAL_H
