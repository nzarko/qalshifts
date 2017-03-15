#ifndef QSHIFTSTABLEITEM_H
#define QSHIFTSTABLEITEM_H

#include <QTableWidgetItem>

namespace Algorithmos {
    const int STIROLE = Qt::UserRole + 2;


class QShiftsTableItem : public QTableWidgetItem
{
public:
    QShiftsTableItem(int type=0);
    void setData(int role, const QVariant &value);

private:
    QMap<int, QBrush> itemBgColor; //item background color depending on shift type.

};
}
#endif // QSHIFTSTABLEITEM_H
