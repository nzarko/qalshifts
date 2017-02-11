#include <QDebug>
#include <QStringList>

#include "qemployeeshiftstable.h"


QEmployeeShiftsTable::QEmployeeShiftsTable(QWidget *parent):
    QTableWidget(parent)
{

    //QIcon icon;
   // QTableWidgetItem *item = new QTableWidgetItem();
    //icon.addFile(QStringLiteral(":/icons/early_shift"),item->sizeHint(),QIcon::Normal, QIcon::On);
    //item->setIcon(icon);
//    item->setData(Qt::DecorationRole,
//                  QPixmap(":/icons/early_shift").scaled(100, 100,
//                                                        Qt::KeepAspectRatio,
//                                                        Qt::SmoothTransformation));
//    item->setBackgroundColor(QColor(Qt::red));
//    item->setText("BR5");
//    setItem(0,0,item);

}

QEmployeeShiftsTable::~QEmployeeShiftsTable()
{

}

void QEmployeeShiftsTable::populate()
{
    setColumnCount(49);
    setRowCount(30);
    setAlternatingRowColors(true);

    QMap<int, QBrush> itemBgColor;
    itemBgColor.insert(Algorithmos::EARLY,QBrush(QColor(Qt::red)));
    itemBgColor.insert(Algorithmos::LATE, QBrush(QColor(Qt::green)));
    itemBgColor.insert(Algorithmos::INTERMITTENT, QBrush(QColor(Qt::blue)));

    Algorithmos::QShiftsCore s_core;
    Algorithmos::QShiftSolver *s_solver = s_core.solver();

    if(s_solver) {
        Shifts shifts = s_solver->initShifts();
        populateVHeader( shifts[0]->bManagers());
        populateVHeader( shifts[0]->bManagers());

    }
    else
        qDebug() << "Solver could not initialized. Check QShiftSolver::init()" << endl;

}

void QEmployeeShiftsTable::populateVHeader(EmployeeMap &e_map)
{
    QMapIterator<Algorithmos::ShiftType, QVector<QEmployee*> > iter (e_map);
    static int r = 0;
    //QStringList e_names;
    QTableWidgetItem *headerItem= Q_NULLPTR;
    while(iter.hasNext()) {
        //Initialize Verticalheader Items.
        iter.next();
        QVector <QEmployee *> e_vector = iter.value();
        for(int i = 0; i < e_vector.size(); i++) {
            headerItem = new QTableWidgetItem();
            headerItem->setText(e_vector[i]->name());
            headerItem->setData(Qt::UserRole,e_vector[i]->toStringList());
            headerItem->setToolTip(e_vector[i]->toString());
            setVerticalHeaderItem(r,headerItem);
            m_eRow.insert(e_vector[i]->ID(),r);
            //e_names << e_vector[i]->name();
            QString str = e_vector[i]->toStringList().join(", ");
            qDebug() << tr("Employee %1 : ").arg(r+1) << str << endl;
            ++r;
        }
    }
    //setVerticalHeaderLabels(e_names);
}

void QEmployeeShiftsTable::populateShiftsTable(Shifts &shifts)
{
    ///TODO : Implement me!!!
}
