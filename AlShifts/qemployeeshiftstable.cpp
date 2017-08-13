#include <QAction>
//#include <QDebug>
#include <QStringList>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QString>
#include <QAction>
#include <QTextStream>
#include <QMessageBox>
#include <QFile>
#include <QLineEdit>
#include <QAbstractItemView>
#include <QLocale>

#include <QHeaderView>
#include <QApplication>

#include <algorithm>
#include <cstdlib>

#include "qemployeeshiftstable.h"

#include "qshifttableitemdelegate.h"
#include "xlsx/xlsxdocument.h"
#include "configuration.h"

int QEmployeeShiftsTable::r = 0;

QEmployeeShiftsTable::QEmployeeShiftsTable(QWidget *parent):
    QTableWidget(parent),
    is_empty(true),
    el_loc(QLocale::Greek,QLocale::Greece),
    m_startDate(QDateTime::currentDateTime())
{

    itemBgColor.insert(Algorithmos::EARLY,QBrush(QColor(Qt::red)));
    itemBgColor.insert(Algorithmos::LATE, QBrush(QColor(Qt::green)));
    itemBgColor.insert(Algorithmos::INTERMITTENT, QBrush(QColor(Qt::blue)));

    branchFullName.insert("BR2", tr("Kalampakas 104"));
    branchFullName.insert("BR3",tr("Pylis 39"));
    branchFullName.insert("BR4",tr("Pylis 98"));
    branchFullName.insert("BR5", tr("Karditsis 54"));

    setColumnCount(ColumnCount);
    setRowCount(RowCount);
    setAlternatingRowColors(true);

    horizontalHeader()->setSectionResizeMode(2,QHeaderView::Stretch);
    verticalHeaderView = new HeaderView(Qt::Vertical, this);
    headerDelegate = new HeaderDelegate(Qt::Vertical, verticalHeaderView);
    verticalHeaderView->setItemDelegate(headerDelegate);
    setVerticalHeader(verticalHeaderView);
    verticalHeader()->setSectionResizeMode(2,QHeaderView::Stretch);
    verticalHeader()->setSectionsClickable(true);

    removeRowAction = new QAction(tr("Remove Row"),this);
    verticalHeader()->addAction(removeRowAction);
    verticalHeader()->setContextMenuPolicy(Qt::ActionsContextMenu);
    connect(removeRowAction, &QAction::triggered,this, &QEmployeeShiftsTable::removeCurrentRow);
    connect(model(),SIGNAL(headerDataChanged(Qt::Orientation,int,int)),this, SLOT(somethingChanged()));
    //connect(verticalHeader(),&QHeaderView::sectionDoubleClicked,this, &QEmployeeShiftsTable::editHeader);

    QShiftTableItemDelegate *i_del = new QShiftTableItemDelegate();
    setItemDelegate(i_del);    


    connect(this, SIGNAL(itemChanged(QTableWidgetItem *)),
            this, SLOT(somethingChanged()));
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

void QEmployeeShiftsTable::clear()
{
    setRowCount(0);
    setColumnCount(0);
    setRowCount(RowCount);
    setColumnCount(ColumnCount);

//    for (int i = 0; i < ColumnCount; ++i) {
//        QTableWidgetItem *item = new QTableWidgetItem;
//        item->setText(QString(QChar('A' + i)));
//        setHorizontalHeaderItem(i, item);
//    }

    setCurrentCell(0, 0);
    QEmployeeShiftsTable::set_r(0);
}

bool QEmployeeShiftsTable::readFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("Aliagas Shifts"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return false;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_6);

    quint32 magic;
    in >> magic;
    if (magic != MagicNumber) {
        QMessageBox::warning(this, tr("Aliagas Shifts"),
                             tr("The file is not a Spreadsheet file."));
        return false;
    }

    clear();

    QTableWidgetItem *curItem = Q_NULLPTR;
    QShiftsTableItem *shItem = Q_NULLPTR;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    for(int col =0; col < ColumnCount && !in.atEnd(); ++col) {
        curItem = new QTableWidgetItem();
        in >> *curItem;
        setHorizontalHeaderItem(col, curItem);
    }
    emtypeRange.clear();
    manRange = {0,0};
    fmanRange = {0, 0};
    feRange = {0, 0};
    QVector<ETRange *> etr_vec = {&manRange, &fmanRange, &feRange };
    int curIndex = 0;

    for(int row = 0; row < RowCount && !in.atEnd(); ++row) {
        curItem = new QTableWidgetItem();
        in >> *curItem;
        setVerticalHeaderItem(row,curItem);
        if(curItem->text() == "Blank") {
            curItem->setText("");
            ///TODO : fill emTypeRange with proper values
            /// so the model be eligible to solve shift changes
            /// v0.8.5
            if(curIndex < 3) {
                etr_vec[curIndex]->endRow = row - 1;
                ++curIndex;
                if(curIndex < 3)
                    etr_vec[curIndex]->startRow = row + 1;
            }
            continue;
        }
        for(int col = 0; col < ColumnCount && !in.atEnd(); ++col) {
            shItem = new QShiftsTableItem();
            in >> *shItem;
            setItem(row,col,shItem);
        }
    }
    file.close();
    QApplication::restoreOverrideCursor();
    emit populationChanged(true);
    qDebug() << "QEmployeeShiftsTable::readFile --> ETRange : ";
    qDebug() << "\t\tManagers : " << manRange.startRow << ", " << manRange.endRow;
    qDebug() << "\t\tFuel Managers : " << fmanRange.startRow << ", " << fmanRange.endRow;
    qDebug() << "\t\tFuel Employees : " << feRange.startRow << ", " << feRange.endRow;

    emtypeRange.insert(Algorithmos::BMANAGER, manRange);
    emtypeRange.insert(Algorithmos::BFUELMANAGER, fmanRange);
    emtypeRange.insert(Algorithmos::FUELMANAGER, feRange);
    //Try to extract m_startDate from first headerItem
    m_startDate = //QDateTime::fromString(horizontalHeaderItem(0)->text(),"ddd dd \nMMM yyyy");
            (QDateTime)el_loc.toDate(horizontalHeaderItem(0)->text(), "ddd dd \nMMM yyyy");
    if(m_startDate.isValid())
        qDebug() << "QEmployeeShiftsTable::readFile-->m_startDate-->" << m_startDate.toString("dd/MM/yyyy") << endl;
    else
        qDebug() << "QEmployeeShiftsTable::readFile : Error parsing Date from first header item" << endl;
    return true;
}

bool QEmployeeShiftsTable::writeFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("Aliagas Shifts"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return false;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_6);

    out << quint32(MagicNumber);

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QTableWidgetItem *curItem;
    for(int col = 0; col < ColumnCount; ++col) { //Save Horizontal header items first
        curItem = horizontalHeaderItem(col);
        if(curItem) {
            out << *curItem;
        }
    }
    for (int row = 0; row < RowCount; ++row) {
        curItem = verticalHeaderItem(row);
        if(curItem && curItem->text() != "")
            out << *curItem;
        else {
            QTableWidgetItem *blankItem = new QTableWidgetItem();
            blankItem->setText("Blank");
            out << *blankItem;
            delete blankItem;
            continue;
        }
        for (int column = 0; column < ColumnCount; ++column) {
            curItem = item(row,column);
            if(curItem)
                out << *curItem;
        }
    }
    file.close();
    QApplication::restoreOverrideCursor();

    //Temporary
    saveShifts();
    return true;
}

QDate QEmployeeShiftsTable::startDate()
{
    return m_startDate.date();
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

StringListArray QEmployeeShiftsTable::createEmployeesSolverData(int col, int shift_type)
{
    qDeleteAll(bsdata_vec);
    bsdata_vec.clear();
    StringListArray res;
    StringList sl;

    int startRow = emtypeRange.value(Algorithmos::BFUELMANAGER).startRow,
            endRow = emtypeRange.value(Algorithmos::BFUELMANAGER).endRow;
    QString txt = item(startRow+2,col)->text();
    startRow = (txt.contains(",") || txt != "BR4") ? (startRow+2) : (startRow + 3);
    //Shifts shifts = s_solver->shifts();
    //int req = shifts[col]->shiftDate().date().dayOfWeek()==7 ? 2 : 3;
    //int req = 0;

    bool is_ok;
    for(int i = startRow; i < endRow; i++) {        
        int st = item(i,col)->data(Algorithmos::STIROLE).toInt(&is_ok);
        QString txt = item(i,col)->text();
        //if(!is_ok) continue;
        if(st == 3) continue;
        if(st == shift_type && (txt.contains("BR2") ||
                                txt.contains("BR3") ||
                                txt.contains("BR5"))) { //Early shift
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
    startRow = emtypeRange.value(Algorithmos::FUELMANAGER).startRow;
    endRow = emtypeRange.value(Algorithmos::FUELMANAGER).endRow;

    for(int i = startRow; i < endRow; i++) {
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

    if(!res.empty() && res.size() >= 3) {
        //Reverse both res and bsdata_vec.
        QVector<BranchSolverData*>result;
        result.reserve( bsdata_vec.size() ); // reserve is new in Qt 4.7
        std::reverse_copy( bsdata_vec.begin(), bsdata_vec.end(), std::back_inserter( result ) );
        bsdata_vec = result;

        std::reverse(res.begin(),res.end());
        StringListArray res_trun;
        //res_trun.resize(3);
        std::copy_n(res.begin(),3,std::back_inserter(res_trun));
        return res_trun;
    }
    return res;
}

void QEmployeeShiftsTable::setStartDate(QDateTime dt)
{
    m_startDate = dt;
}

int QEmployeeShiftsTable::columnForDate(const QDate &date)
{
    qint64 duration = -date.daysTo(m_startDate.date());
    Q_ASSERT_X(duration >= 0, "Division", "Duration must be positive number!!");
    lldiv_t result = std::div(duration,(qint64)7);
    qDebug() << "Column for Date : " << date.toString("dd/MM/yyyy") << " ==> " << result.quot*7 << endl;
    return result.quot * 7;
}

QDate QEmployeeShiftsTable::dateForColumn(int col)
{
    QDate dt = QDate::currentDate();
    if(is_empty) {
        qDebug() << "Shifts table is empty. Fill it first then try again" << endl;
    }

    QString str_date= horizontalHeaderItem(col)->text();
    dt = el_loc.toDate(str_date,"ddd dd \nMMM yyyy");

    //dt = QDate::fromString(str_date,"ddd dd \nMMM yyyy");
    if(!dt.isValid())
    {
        qDebug() << "Could not read date for column " << col << "!! See QEmployeeShiftsTable::dateForColumn(int col)"
                 <<" for more info" << endl;
        dt.setDate(1979,1,1);
    }
    return dt;
}

QString QEmployeeShiftsTable::currentLocation()
{
    return "R:"+QString::number(currentRow()) + " C:" + QString::number(currentColumn());
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
    setCurrentCell(0,0);

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
//    is_in_solve_fun = true;
//    QStringList solution;
//    //QMapIterator<Algorithmos::EmployeeType, ETRange> m_iter(emtypeRange);
//    for(int j=0; j < columnCount(); j++){
//        solution = Algorithmos::QShiftSolver::solve_branch_shifts(createSolverData(
//                                                                      emtypeRange.value(Algorithmos::BMANAGER),
//                                                                      j,(int)Algorithmos::EARLY));
//        qDebug() << "Early solution , column " << j << " : " << solution << endl;
//        if ( !solution.empty()) {
//            for(int i=0; i<solution.size(); i++) {
//                item(bsdata_vec[i]->row, bsdata_vec[i]->col)->setText(solution[i]);
//            }
//        }
//        solution.clear();
//        solution = Algorithmos::QShiftSolver::solve_branch_shifts(createSolverData(
//                                                                      emtypeRange.value(Algorithmos::BMANAGER),
//                                                                      j,(int)Algorithmos::LATE));
//        qDebug() << "Late solution , column " << j << " : " << solution << endl;
//        if(!solution.empty()) {
//            for(int i=0; i<solution.size(); i++) {
//                item(bsdata_vec[i]->row, bsdata_vec[i]->col)->setText(solution[i]);
//            }
//        }
//    }
//    is_in_solve_fun = false;
    for(int j = 0; j < columnCount(); j++)
        solve(j);    
}

void QEmployeeShiftsTable::solve(int col)
{
    is_in_solve_fun = true;
    QMapIterator<Algorithmos::EmployeeType, ETRange> m_iter(emtypeRange);
    while(m_iter.hasNext()) {
        m_iter.next();
        ETRange range = m_iter.value();        
        if(m_iter.key() == Algorithmos::BMANAGER) {

            QStringList solution = Algorithmos::QShiftSolver::solve_branch_shifts(createSolverData(
                                                                                      /*emtypeRange.value(Algorithmos::BMANAGER),*/
                                                                                      range,
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
                                                                          /*emtypeRange.value(Algorithmos::BMANAGER),*/
                                                                          range,
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
        }
    }
    solveBR4(col);
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
   // QString txt; //in case that one of the items is DAYOFF
    int data1 = item1->data(Algorithmos::STIROLE).toInt();
    int data2 = item2->data(Algorithmos::STIROLE).toInt();
    if(data1 == data2 )
        return;
    else {
        if (data1 == (int) Algorithmos::DAYOFF) {
            item1->setText(item1->data(Qt::UserRole+1).toStringList().join(", "));
            item2->setText("");
        }
        if (data2 == (int) Algorithmos::DAYOFF) {
            item2->setText(item2->data(Qt::UserRole+1).toStringList().join(", "));
            item1->setText("");
        }
    }
    item1->setData(Algorithmos::STIROLE,item2->data(Algorithmos::STIROLE));
    item2->setData(Algorithmos::STIROLE, data1);

    updateToolTip(item1);
    updateToolTip(item2);

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

void QEmployeeShiftsTable::solveBR4(int col)
{
    ETRange range = {fmanRange.startRow, fmanRange.startRow + 3};

    auto count = 0;
    for(auto i = range.startRow; i < range.endRow; ++i) {
        QTableWidgetItem *theItem = item(i,col);
        if(theItem) {
            if (theItem->data(Algorithmos::STIROLE).toInt() == 3)
                continue;
            if(theItem->data(Qt::UserRole + 1).toStringList().contains("BR4") && count < 2) {
                theItem->setText("BR4");
                ++count;
            }
        }

    }
}

void QEmployeeShiftsTable::rearrangeEmployeesShift()
{    

    //loadBFuelShifts();
    // Now find a solution
    std::vector<Algorithmos::ShiftType> sv = {Algorithmos::EARLY,
                                              Algorithmos::LATE,
                                              Algorithmos::INTERMITTENT };
    for(int col =0; col < columnCount(); col++) {
        for(auto x : sv) {
            QString shift_name = Algorithmos::shiftName(x);
            StringListArray sla = createEmployeesSolverData( col,(int)x);
            if(sla.empty()) continue;
            Algorithmos::QShiftSolver::set_required_branches({"BR2", "BR3", "BR5"});
            QStringList solution = Algorithmos::QShiftSolver::solve_branch_shifts(sla);
            qDebug() << "Solution for column " << col << shift_name <<" shift :" << solution << endl;
            if(!solution.empty()) {
                Q_ASSERT(solution.size()<=bsdata_vec.size());
                int i;
                for(i=0; i<solution.size(); i++) {
                    item(bsdata_vec[i]->row, bsdata_vec[i]->col)->setText(solution[i]);
                }
                if(solution.size() < bsdata_vec.size()) {
                    for(i=solution.size(); i < bsdata_vec.size(); ++i) {
                        QTableWidgetItem *tw_item  = item(bsdata_vec[i]->row, bsdata_vec[i]->col);
                        tw_item->setData(Algorithmos::STIROLE,Algorithmos::AVAILABLE);
                        QString txt = tr("Available \n %1").arg(tw_item->text());
                        tw_item->setText(txt);
                    }
                }
            } else {
                QTableWidgetItem *tw_item;
                for(int i =0; i < bsdata_vec.size(); i++) {
                    tw_item = item(bsdata_vec[i]->row, bsdata_vec[i]->col);
                    QString txt = tw_item->data(Qt::UserRole+1).toStringList().join(",");
                    tw_item->setText(txt);
                }
            }
        }
    }

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
            headerItem->setData(EID, e_vector[i]->ID());
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
        headerItem->setData(EmployeeType, (int)em_vec[i]->employeeType());
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

        vHeaderItem->setText(/*date_time.toString("ddd dd \nMMM yyyy")*/
                             el_loc.toString(date_time,"ddd dd \nMMM yyyy"));
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
    QEmployeeShiftsTable::r= val;
}

void QEmployeeShiftsTable::saveShifts()
{
    QFile file("mgrshmat.bmrx"); //Managers files (binary mode)
    if(!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this,tr("Error writing!"),
                              tr("Error while trying to write %1 file").arg(file.fileName()),
                              QMessageBox::Ok);
    }
    QDataStream out(&file);
    //For Managers
    for(auto row = 0; row < 7; ++row)
        for(auto col = 0; col < ColumnCount; ++col) {
            out << item(row,col)->data(Algorithmos::STIROLE).toInt();
        }

    qDebug() << tr("File %1 saved succesfull!").arg(file.fileName()) << endl;
    file.close();
    /* *****************
     * Fuel Managers
     * *****************/
    QFile filefm("fmgrshmat.bmrx");
    if(!filefm.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this,tr("Error writing!"),
                              tr("Error while trying to write %1 file").arg(filefm.fileName()),
                              QMessageBox::Ok);
    }
    QDataStream outfm(&filefm);
    for(auto row = 8; row < 14; ++row)
        for(auto col = 0; col < ColumnCount; ++col) {
            outfm << item(row,col)->data(Algorithmos::STIROLE).toInt();
        }
    qDebug() << "File " << filefm.fileName() << " saved succesfull!" << endl;
    filefm.close();

    /* *****************
     * Fuel Employees
     * *****************/
    QFile filefe("feemplshmat.bmrx");
    if(!filefe.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this,tr("Error writing!"),
                              tr("Error while trying to write %1 file").arg(filefe.fileName()),
                              QMessageBox::Ok);
    }
    QDataStream outfe(&filefe);
    for(auto row = 15; row < 22; ++row)
        for(auto col = 0; col < ColumnCount; ++col) {
            outfe << item(row,col)->data(Algorithmos::STIROLE).toInt();
        }
    qDebug() << "File " << filefe.fileName() << " saved succesfull!" << endl;
    filefe.close();
}



void QEmployeeShiftsTable::loadBFuelShifts()
{
    QFile f(s_solver->bfmMatrixFile());
    if(!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, tr("Error reading solution!"),
                              tr("Could not read %1 file. Check your installation!"));
        return;
    }

    QString line;
    QStringList values;
    QTextStream ts(&f);
    while (!ts.atEnd()) {
        line = ts.readLine();
        values = line.split(":",QString::SkipEmptyParts);
        Q_ASSERT_X(values.size()==3,"Reading", "Values are less than expected!");
        item(values[0].toInt(),values[1].toInt())->setData(Algorithmos::STIROLE,values[2].toInt());
    }
    f.close();
}

void QEmployeeShiftsTable::updateShifts(int col)
{
    ///TODO : Implement me!!
}

void QEmployeeShiftsTable::updateShifts()
{
    for(auto j = 0; j < columnCount(); ++j)
        updateShifts(j);
}

void QEmployeeShiftsTable::editHeader(int row)
{
    qDebug() << verticalHeaderItem(row)->text() << " --> clicked. " << endl;
    QHeaderView *header = verticalHeader();
    QLineEdit *le = new QLineEdit(verticalHeader()->viewport());
    connect(le, &QLineEdit::editingFinished, [=]() {
        verticalHeaderItem(row)->setText(le->text());
        le->setHidden(true);
        le->deleteLater();
    });
    le->setAlignment(Qt::AlignTop);
    le->setHidden(true);
    scrollToItem(verticalHeaderItem(row));
    auto edit_geometry = le->geometry();
    edit_geometry.setWidth(verticalHeader()->sectionSize(row));
    edit_geometry.setTop(header->sectionPosition(row));
    edit_geometry.setHeight(header->sectionSize(row));
    edit_geometry.setLeft(0);
    edit_geometry.setWidth(header->width());
    edit_geometry.adjust(1,1,-1,-1);
    //edit_geometry.moveLeft(verticalHeader()->sectionViewportPosition(row));
    le->setGeometry(edit_geometry);
    le->setFrame(false);
    le->setText(verticalHeaderItem(row)->text());
    le->setHidden(false);
    le->setFocus();
    le->selectAll();
    //le->deleteLater();
}

void QEmployeeShiftsTable::doneEditing()
{

}

void QEmployeeShiftsTable::forceIntermittent()
{
    QList<QTableWidgetItem *> selected = selectedItems();
    for(auto sitem : selected ) {
        sitem->setData(Algorithmos::STIROLE, (int)Algorithmos::INTERMITTENT);
    }
}

void QEmployeeShiftsTable::showBranchFullNames(bool toggled)
{
    /* TODO: Implement me! */
    int shift_type =(int) Algorithmos::AVAILABLE;
    for(int i=0; i < RowCount; ++i) {
        for(int j = 0; j < ColumnCount; ++j) {
            QTableWidgetItem *s_item = item(i,j);
            if(s_item) {
                if(s_item->data(Algorithmos::STIROLE).toInt() != shift_type) {
                    if(toggled) {
                        QString code = s_item->text();
                        s_item->setText(branchFullName.value(code));
                    } else{
                        QString code = s_item->text();
                        s_item->setText(branchFullName.key(code));
                    }

                }
            }
        }
    }

}

void QEmployeeShiftsTable::updateVHeader()
{
    qDebug() << "QEmployeeShiftsTable::updateVHeader --> ";
    if(!is_empty) {
        //Shifts shifts = s_solver->initShifts(m_startDate);
        s_core.init();
        set_r(0);
        Q_ASSERT_X(QEmployeeShiftsTable::r == 0, "r initialization", "r does not initialized properly");
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
        qDebug() << "Changes sould be visible now!" << endl;
    } else
        s_core.init();
}

void QEmployeeShiftsTable::updateToolTip(QTableWidgetItem *item)
{
    using Algorithmos::shiftName;
    Algorithmos::ShiftType s_type = (Algorithmos::ShiftType)item->data(Algorithmos::STIROLE).toInt();
    QString tool_tip = tr("Shift type : %1").arg(shiftName(s_type));
    tool_tip+= tr("\nAvailable branches : %1").arg(item->data(Qt::UserRole+1).toStringList().join(", "));
    item->setToolTip(tool_tip);
}

void QEmployeeShiftsTable::clearDOText()
{
    QShiftsTableItem *sItem = Q_NULLPTR;
    for(int i = 0; i < RowCount; ++i)
        for(int j = 0; j < ColumnCount; ++j) {
            sItem = dynamic_cast<QShiftsTableItem *>(item(i,j));
            if(sItem) {
                if(sItem->data(Algorithmos::STIROLE).toInt() ==(int) Algorithmos::DAYOFF)
                    sItem->setText("");
            }
        }
}

void QEmployeeShiftsTable::somethingChanged()
{
    emit modified();
}

void QEmployeeShiftsTable::removeCurrentRow()
{
//    QModelIndex index = this->indexAt(pos);
//    qDebug() << "Index at right click : " << index.row() << endl;
    //HeaderView *hv = qobject_cast<HeaderView *> (verticalHeader());
    this->removeRow(verticalHeaderView->getClickedRow());
}

QEmployeeShiftsTable::EmployeeTypeRowRange QEmployeeShiftsTable::EmployeeTypeRowRange::operator +(const int k)
{
    EmployeeTypeRowRange etr;
    etr.startRow = this->startRow + k;
    etr.endRow = this->endRow + k;
    return etr;
}

QEmployeeShiftsTable::EmployeeTypeRowRange QEmployeeShiftsTable::EmployeeTypeRowRange::operator -(const int k)
{
    EmployeeTypeRowRange etr;
    etr.startRow = this->startRow - k;
    etr.endRow = this->endRow - k;
    return etr;
}

QEmployeeShiftsTable::EmployeeTypeRowRange &QEmployeeShiftsTable::EmployeeTypeRowRange::operator +=(const int k)
{
    this->startRow += k;
    this->endRow += k;
    return *this;
}

QEmployeeShiftsTable::EmployeeTypeRowRange &QEmployeeShiftsTable::EmployeeTypeRowRange::operator -=(const int k)
{
    this->endRow -= k;
    this->startRow -= k;
    return *this;
}

QDebug operator <<(QDebug dbg, const QEmployeeShiftsTable::EmployeeTypeRowRange e)
{
    dbg.nospace() << "Start Row : " << e.startRow << ", End Row : " << e.endRow;
    return dbg;
}
