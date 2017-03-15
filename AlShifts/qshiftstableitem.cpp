#include "qshiftstableitem.h"
#include "qshiftscore.h"

namespace Algorithmos {


QShiftsTableItem::QShiftsTableItem(int type)
    : QTableWidgetItem(type)
{
    itemBgColor.insert(Algorithmos::EARLY,QBrush(QColor(Qt::red)));
    itemBgColor.insert(Algorithmos::LATE, QBrush(QColor(Qt::green)));
    itemBgColor.insert(Algorithmos::INTERMITTENT, QBrush(QColor(Qt::cyan)));
}

void QShiftsTableItem::setData(int role, const QVariant &value)
{
    if(role == STIROLE) {
        setBackground(itemBgColor.value(value.toInt()));
    }
    QTableWidgetItem::setData(role,value);
}

}
