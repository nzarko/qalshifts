#include <QDebug>
#include <QStringList>

#include <QHeaderView>
#include <QApplication>
#include "qemployeeshiftstable.h"

int QEmployeeShiftsTable::r = 0;

QEmployeeShiftsTable::QEmployeeShiftsTable(QWidget *parent):
    QTableWidget(parent),
    is_empty(true)
{

    itemBgColor.insert(Algorithmos::EARLY,QBrush(QColor(Qt::red)));
    itemBgColor.insert(Algorithmos::LATE, QBrush(QColor(Qt::green)));
    itemBgColor.insert(Algorithmos::INTERMITTENT, QBrush(QColor(Qt::blue)));

    setColumnCount(49);
    setRowCount(30);
    setAlternatingRowColors(true);

    horizontalHeader()->setSectionResizeMode(2,QHeaderView::Stretch);
    verticalHeader()->setSectionResizeMode(2,QHeaderView::Stretch);
    //resizeRowsToContents();
    //resizeColumnsToContents();
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
    Algorithmos::QShiftsCore s_core;
    Algorithmos::QShiftSolver *s_solver = s_core.solver();

    if(s_solver) {
        Shifts shifts = s_solver->initShifts();
        populateVHeader( shifts[0]->bManagers());
        ///TODO : Uncomment the following lines when
        /// shifts for fuel managers and employees are ready
        //populateVHeader( shifts[0]->bfManagers());
        //populateVHeader(shifts[0]->fEmployees());
        populateShiftsTable(shifts);
        is_empty = false;
        emit populationChanged(true);

    }
    else
        qDebug() << "Solver could not initialized. Check QShiftSolver::init()" << endl;

}

bool QEmployeeShiftsTable::isEmpty()
{
    return is_empty;
}

void QEmployeeShiftsTable::clearShifts()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    setRowCount(0);
    setColumnCount(0);
    is_empty = true;
    emit populationChanged(false);

    setColumnCount(49);
    setRowCount(30);
    setAlternatingRowColors(true);
    set_r(0);

    QApplication::restoreOverrideCursor();
}

void QEmployeeShiftsTable::populateVHeader(EmployeeMap &e_map)
{
    QMapIterator<Algorithmos::ShiftType, QVector<QEmployee*> > iter (e_map);

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
    ///TODO : Implement me!!

   // QMapIterator<Algorithmos::ShiftType, QVector<QEmployee*> > iter (e_map);
    for(int j = 0; j < shifts.size(); j++) {
        QDateTime date_time = shifts[j]->shiftDate();
        QTableWidgetItem *vHeaderItem = new QTableWidgetItem();
        vHeaderItem->setText(date_time.toString("ddd dd \nMMM yyyy"));
        vHeaderItem->setTextAlignment(Qt::AlignCenter);

        setHorizontalHeaderItem(j,vHeaderItem);
        EmployeeMap m_map = shifts[j]->bManagers(); //Managers
        EmployeeMap fm_map = shifts[j]->bfManagers(); // Fuel Managers
        EmployeeMap e_map = shifts[j]->fEmployees(); //Fuel Employees
        if (m_map.size() > 0) {
            QMapIterator<Algorithmos::ShiftType, QVector<QEmployee*> > m_iter(m_map);
            //QStringList e_names;
            QTableWidgetItem *s_item= Q_NULLPTR;
            while(m_iter.hasNext()) {
                //Initialize Verticalheader Items.
                m_iter.next();
                QVector <QEmployee *> e_vector = m_iter.value();
                for(int i = 0; i < e_vector.size(); i++) {
                    s_item = new QTableWidgetItem();
                    s_item->setText(e_vector[i]->branches().join(", "));
                    s_item->setToolTip(shiftName(m_iter.key()));
                    QBrush brush = itemBgColor.value((int)m_iter.key());
                    //qDebug() << brush.color().toRgb() << endl;
                    s_item->setBackground(brush);
                    setItem(m_eRow.value(e_vector[i]->ID()),j,s_item);
                }
            }
        }
    }
}

void QEmployeeShiftsTable::set_r(int val)
{
    r= val;
}
