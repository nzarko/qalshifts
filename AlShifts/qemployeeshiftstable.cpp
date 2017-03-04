#include <QDebug>
#include <QStringList>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QString>
#include <QAction>
#include <QTextStream>

#include <QHeaderView>
#include <QApplication>
#include "qemployeeshiftstable.h"

#include "qshifttableitemdelegate.h"
#include "xlsx/xlsxdocument.h"

int QEmployeeShiftsTable::r = 0;

QEmployeeShiftsTable::QEmployeeShiftsTable(QWidget *parent):
    QTableWidget(parent),
    is_empty(true),
    m_startDate(QDateTime::currentDateTime())
{

    itemBgColor.insert(Algorithmos::EARLY,QBrush(QColor(Qt::red)));
    itemBgColor.insert(Algorithmos::LATE, QBrush(QColor(Qt::green)));
    itemBgColor.insert(Algorithmos::INTERMITTENT, QBrush(QColor(Qt::blue)));

    setColumnCount(49);
    setRowCount(30);
    setAlternatingRowColors(true);

    horizontalHeader()->setSectionResizeMode(2,QHeaderView::Stretch);
    verticalHeader()->setSectionResizeMode(2,QHeaderView::Stretch);

    QShiftTableItemDelegate *i_del = new QShiftTableItemDelegate();
    setItemDelegate(i_del);    

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
        Shifts shifts = s_solver->initShifts(m_startDate);
        manRange.startRow = r;
        populateVHeader(s_core.branchManagers());
        manRange.endRow = r;
        r++;
        fmanRange.startRow = r;
        populateVHeader(s_core.branchFuelManagers());
        fmanRange.endRow = r;
        r++;
        feRange.startRow = r;
        populateVHeader(s_core.fuelEmployees());
        feRange.endRow = r;

        qDebug() << "managers : (" << manRange.startRow << ", " << manRange.endRow << ")" << '\n'
                 << "fuel managers : (" << fmanRange.startRow << ", " << fmanRange.endRow << ")" << '\n'
                 << "fuel employees : (" << feRange.startRow << ", " << feRange.endRow << ")"
                 << endl;

        emtypeRange.insert(Algorithmos::BMANAGER,manRange);
        emtypeRange.insert(Algorithmos::BFUELMANAGER,fmanRange);
        emtypeRange.insert(Algorithmos::FUELMANAGER, feRange);
        populateShiftsTable(shifts);
        is_empty = false;
        emit populationChanged(true);
        //Occurs when the user change shift from Cell ComboBox
        //connect(this, SIGNAL(cellChanged(int,int)),SLOT(updateCell(int,int)));
        //connect(this, &QEmployeeShiftsTable::itemChanged, this,&QEmployeeShiftsTable::updateItemColumn);
        //Enable/Disable swap shifts action depending of how many items are selected
        // If there are two ,action be enabled.
        connect(this, SIGNAL(itemSelectionChanged()), SLOT(updateActions()));

    }
    else
        qDebug() << "Solver could not initialized. Check QShiftSolver::init()" << endl;

}

bool QEmployeeShiftsTable::isEmpty()
{
    return is_empty;
}

StringListArray QEmployeeShiftsTable::createSolverData(ETRange range,int col,int shift_type)
{
    qDeleteAll(bsdata_vec);
    bsdata_vec.clear();
    StringListArray res;
    StringList sl;
    for(int i = range.startRow; i < range.endRow; i++) {
        int st = item(i,col)->data(Algorithmos::STIROLE).toInt();
        if(st == 3) continue;
        if(st == shift_type) { //Early shift
            bsdata = new BranchSolverData;
            bsdata->row = i;
            bsdata->col = col;
            bsdata->branches = //verticalHeaderItem(i)->data(Qt::UserRole).toStringList();
                    item(i,col)->data(Qt::UserRole+1).toStringList();
            sl =Algorithmos::qsl_to_sl(bsdata->branches);
            res.push_back(sl);
            bsdata_vec.push_back(bsdata);
        }
    }
    return res;
}

void QEmployeeShiftsTable::setStartDate(QDateTime &dt)
{
    m_startDate = dt;
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

void QEmployeeShiftsTable::solve()
{
    is_in_solve_fun = true;
    QStringList solution;
    //QMapIterator<Algorithmos::EmployeeType, ETRange> m_iter(emtypeRange);
    for(int j=0; j < columnCount(); j++){
        solution = Algorithmos::QShiftSolver::solve_branch_shifts(createSolverData(
                                                                      emtypeRange.value(Algorithmos::BMANAGER),
                                                                      j,(int)Algorithmos::EARLY));
        qDebug() << "Early solution , column " << j << " : " << solution << endl;
        if ( !solution.empty()) {
            for(int i=0; i<solution.size(); i++) {
                item(bsdata_vec[i]->row, bsdata_vec[i]->col)->setText(solution[i]);
            }
        }
        solution.clear();
        solution = Algorithmos::QShiftSolver::solve_branch_shifts(createSolverData(
                                                                      emtypeRange.value(Algorithmos::BMANAGER),
                                                                      j,(int)Algorithmos::LATE));
        qDebug() << "Late solution , column " << j << " : " << solution << endl;
        if(!solution.empty()) {
            for(int i=0; i<solution.size(); i++) {
                item(bsdata_vec[i]->row, bsdata_vec[i]->col)->setText(solution[i]);
            }
        }
    }
    is_in_solve_fun = false;
}

void QEmployeeShiftsTable::solve(int col)
{
    is_in_solve_fun = true;
    QStringList solution = Algorithmos::QShiftSolver::solve_branch_shifts(createSolverData(
                                                                              emtypeRange.value(Algorithmos::BMANAGER),
                                                                                  col,(int)Algorithmos::EARLY));
    if(!solution.empty()) {
        for(int i=0; i<solution.size(); i++) {
            item(bsdata_vec[i]->row, bsdata_vec[i]->col)->setText(solution[i]);
        }
    } else {
        QTableWidgetItem *tw_item;
        for(int i =0; i < bsdata_vec.size(); i++) {
            tw_item = item(bsdata_vec[i]->row, bsdata_vec[i]->col);
            QString txt = tw_item->data(Qt::UserRole+1).toStringList().join(",");
            tw_item->setText(txt);
        }
    }
    solution = Algorithmos::QShiftSolver::solve_branch_shifts(createSolverData(
                                                                  emtypeRange.value(Algorithmos::BMANAGER),
                                                                  col,(int)Algorithmos::LATE));
    if(!solution.empty()) {
        for(int i=0; i<solution.size(); i++) {
            item(bsdata_vec[i]->row, bsdata_vec[i]->col)->setText(solution[i]);
        }
    }else {
        QTableWidgetItem *tw_item;
        for(int i =0; i < bsdata_vec.size(); i++) {
            tw_item = item(bsdata_vec[i]->row, bsdata_vec[i]->col);
            QString txt = tw_item->data(Qt::UserRole+1).toStringList().join(",");
            tw_item->setText(txt);
        }
    }
    is_in_solve_fun = false;
}

void QEmployeeShiftsTable::updateCell(int /*row*/, int col)
{
    if(!is_empty)
        solve(col);
}

void QEmployeeShiftsTable::updateItemColumn(QTableWidgetItem *item)
{
    if (is_empty == false && is_in_solve_fun == false)
        solve(item->column());
}

void QEmployeeShiftsTable::swapShifts(QTableWidgetItem *item1, QTableWidgetItem *item2)
{
    int data1 = item1->data(Algorithmos::STIROLE).toInt();
    item1->setData(Algorithmos::STIROLE,item2->data(Algorithmos::STIROLE));
    item2->setData(Algorithmos::STIROLE, data1);
    solve(item1->column());
}

void QEmployeeShiftsTable::updateActions()
{
    QAction* a = actions()[0];
    if(selectedItems().size() == 2 &&
            selectedItems()[0]->column() == selectedItems()[1]->column()) {
        a->setEnabled(true);
        //swapShifts(selectedItems()[0], selectedItems()[1]);
    } else
        a->setEnabled(false);
}

void QEmployeeShiftsTable::swapShifts()
{
    swapShifts(selectedItems()[0], selectedItems()[1]);
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
    for(int i = 0; i < em_vec.size(); i++) {
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
    Q_UNUSED(m)
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
                QString tool_tip = tr("Shift type : %1").arg(shiftName(m_iter.key()));
                tool_tip+= tr("\nAvailable branches : %1").arg(e_vector[i]->branches().join(", "));
                s_item->setToolTip(tool_tip);
                s_item->setData(Algorithmos::STIROLE,(int)m_iter.key());
                s_item->setData(Qt::UserRole+1,e_vector[i]->branches());
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
