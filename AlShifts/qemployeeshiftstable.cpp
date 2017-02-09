#include "qemployeeshiftstable.h"

QEmployeeShiftsTable::QEmployeeShiftsTable(QWidget *parent):
    QTableWidget(parent)
{
    setColumnCount(49);
    setRowCount(30);
    setAlternatingRowColors(true);
    QIcon icon;
    QTableWidgetItem *item = new QTableWidgetItem();
    icon.addFile(QStringLiteral(":/icons/early_shift"),item->sizeHint(),QIcon::Normal, QIcon::On);
    //item->setIcon(icon);
    item->setData(Qt::DecorationRole,
                  QPixmap(":/icons/early_shift").scaled(100, 100,
                                                        Qt::KeepAspectRatio,
                                                        Qt::SmoothTransformation));
    setItem(0,0,item);

}

QEmployeeShiftsTable::~QEmployeeShiftsTable()
{

}
