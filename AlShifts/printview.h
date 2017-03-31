#ifndef PRINTVIEW_H
#define PRINTVIEW_H

#include <QTableView>
class QPrinter;

class PrintView : public QTableView
{
    Q_OBJECT

public:
    PrintView();

public Q_SLOTS:
    void print(QPrinter *p);
};

#endif // PRINTVIEW_H
