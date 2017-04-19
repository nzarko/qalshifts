#ifndef PRINTVIEW_H
#define PRINTVIEW_H

#include <QTableView>

#include "abstractprintview.h"

class PrintView : public AbstractPrintView
{
    Q_OBJECT

public:
    PrintView();

public Q_SLOTS:
    void print(QPrinter *p);
};

#endif // PRINTVIEW_H
