#ifndef ABSTRACTPRINTVIEW_H
#define ABSTRACTPRINTVIEW_H

#include <QTableView>
#include <QPrinter>

class AbstractPrintView : public QTableView
{
public:
    AbstractPrintView();
public Q_SLOTS:
    virtual void print(QPrinter *) = 0;
};

#endif // ABSTRACTPRINTVIEW_H
