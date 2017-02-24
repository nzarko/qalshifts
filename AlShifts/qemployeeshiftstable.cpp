#include <QDebug>
#include <QStringList>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QString>
#include <QTextStream>

#include <QHeaderView>
#include <QApplication>
#include "qemployeeshiftstable.h"
#include "qshiftstableitem.h"
#include "xlsx/xlsxdocument.h"

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
    s_solver = s_core.solver();

    if(s_solver) {
        Shifts shifts = s_solver->initShifts();
        populateVHeader(s_core.branchManagers());
        r++;
        populateVHeader(s_core.branchFuelManagers());
        r++;
        populateVHeader(s_core.fuelEmployees());

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

void QEmployeeShiftsTable::print()
{
    ///TODO : Add your code here!!
}

void QEmployeeShiftsTable::printPreview()
{
    ///TODO : Implement me!!!
}

QString QEmployeeShiftsTable::exportToHtml()
{
    ///TODO : Needs improvements.
    QString strStream;
    QString strTitle = tr("Aliagas Shifts Table from %1 to %2")
            .arg(horizontalHeaderItem(0)->text())
            .arg(horizontalHeaderItem(48)->text());

    QTextStream out(&strStream);

    const int rCount = rowCount();
    const int cCount = columnCount();

    out <<  "<html>\n"
            "<head>\n"
            "<meta Content=\"Text/html; charset=Windows-1251\">\n"
         <<  QString("<title>%1</title>\n").arg(strTitle)
          <<  "</head>\n"
              "<body bgcolor=#ffffff link=#5000A0>\n"
              "<table border=1 cellspacing=0 cellpadding=2>\n";

    // headers
    out << "<thead><tr bgcolor=#f0f0f0>";
    for (int column = 0; column < cCount; column++)
        if (!isColumnHidden(column))
            out << QString("<th>%1</th>").arg(this->model()->headerData(column, Qt::Horizontal).toString());
    out << "</tr></thead>\n";

    // data table
    for (int row = 0; row < rCount; row++) {
        out << "<tr>";
        for (int column = 0; column < cCount; column++) {
            if (!isColumnHidden(column)) {
                QString data = this->model()->data(this->model()->index(row, column)).toString().simplified();
                out << QString("<td bkcolor=0>%1</td>").arg((!data.isEmpty()) ? data : QString("&nbsp;"));
            }
        }
        out << "</tr>\n";
    }
    out <<  "</table>\n"
            "</body>\n"
            "</html>\n";

    qDebug() << strStream << endl;
    return strStream;
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

void QEmployeeShiftsTable::populateVHeader(const QVector<Algorithmos::QEmployee *> &em_vec)
{
    for(size_t i = 0; i < em_vec.size(); i++) {
        QTableWidgetItem *headerItem = new QTableWidgetItem();
        headerItem->setText(em_vec[i]->name());
        headerItem->setData(Qt::UserRole, em_vec[i]->toStringList());
        headerItem->setToolTip(em_vec[i]->toString());
        setVerticalHeaderItem(r, headerItem);
        m_eRow.insert(em_vec[i]->ID(),r);
        QString str = em_vec[i]->toStringList().join(", ");
        qDebug() << tr("Employee %1 : ").arg(r+1) << str
                 << "Row : " << m_eRow.value(em_vec[i]->ID()) <<  endl;
        ++r;
    }
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
        fillTableByEmployeeCategory(m_map,j);
        fillTableByEmployeeCategory(fm_map,j);
        ///TODO : uncomment the following line when ready
        fillTableByEmployeeCategory(e_map,j);
    }
}

void QEmployeeShiftsTable::populateShiftsTable(const UBlas::matrix<int> &m)
{

}

void QEmployeeShiftsTable::fillTableByEmployeeCategory(EmployeeMap &m_map, int j)
{
    Algorithmos::ShiftType s_type; //Used to determine if an item should display text or not
    //depending on which shift type (e.g if s_type is DAYOFF the item does not need to display
    // in which branches the employee belongs.
    if (m_map.size() > 0) {
        QMapIterator<Algorithmos::ShiftType, QVector<QEmployee*> > m_iter(m_map);
        //QStringList e_names;
        Algorithmos::QShiftsTableItem *s_item= Q_NULLPTR;
        while(m_iter.hasNext()) {
            //Initialize Verticalheader Items.
            m_iter.next();
            QVector <QEmployee *> e_vector = m_iter.value();
            for(int i = 0; i < e_vector.size(); i++) {
                s_item = new Algorithmos::QShiftsTableItem();
                s_type = m_iter.key();
                if (s_type != Algorithmos::DAYOFF)
                    s_item->setText(e_vector[i]->branches().join(", "));
                s_item->setToolTip(shiftName(m_iter.key()));
                s_item->setData(Algorithmos::STIROLE,(int)m_iter.key());
//                  //QBrush brush = itemBgColor.value((int)m_iter.key());
//                  //qDebug() << brush.color().toRgb() << endl;
//                  //s_item->setBackground(brush);
                setItem(m_eRow.value(e_vector[i]->ID()),j,s_item);
            }
        }
    }
}

void QEmployeeShiftsTable::set_r(int val)
{
    r= val;
}
