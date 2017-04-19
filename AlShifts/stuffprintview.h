#ifndef STUFFPRINTVIEW_H
#define STUFFPRINTVIEW_H

#include "abstractprintview.h"

class StuffPrintView : public AbstractPrintView
{
public:
    StuffPrintView();

public Q_SLOTS:
    void print(QPrinter *p);

private:
void init();
void createSkeleton(bool forFuelEmployees);
int startRow;

};

#endif // STUFFPRINTVIEW_H
