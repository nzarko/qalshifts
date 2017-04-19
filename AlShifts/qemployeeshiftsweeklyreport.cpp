#include <QFont>
#include <QDate>
#include <QPrintDialog>
#include <QPrinter>
#include <QPrintPreviewDialog>
#include <QProgressDialog>
#include <QPainter>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QTranslator>
#include <QProcess>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#include <algorithm>

#include "qemployeeshiftsweeklyreport.h"
#include "qemployeeshiftstable.h"
#include "ui_qemployeeshiftsweeklyreport.h"
#include <weekreportinfo.h>

QEmployeeShiftsWeeklyReport::QEmployeeShiftsWeeklyReport(QEmployeeShiftsTable *empl, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QEmployeeShiftsWeeklyReport),
    pEST(empl)
{
    ui->setupUi(this);
    weekTable = ui->weekReportTableW;
    verticalHeaderView = new HeaderView(Qt::Vertical, weekTable);
    headerDelegate = new HeaderDelegate(Qt::Vertical, verticalHeaderView);
    verticalHeaderView->setItemDelegate(headerDelegate);
    verticalHeaderView->setSectionsClickable(true);
    weekTable->setVerticalHeader(verticalHeaderView);

    insertRowAction = new QAction(tr("Insert row"),this);
    removeRowAction = new QAction(tr("Remove row"),this);
    weekTable->verticalHeader()->addAction(insertRowAction);
    weekTable->verticalHeader()->addAction(removeRowAction);
    weekTable->verticalHeader()->setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(insertRowAction, &QAction::triggered, this, &QEmployeeShiftsWeeklyReport::insertEmployeeRow);
    connect(removeRowAction, &QAction::triggered, this, &QEmployeeShiftsWeeklyReport::removeEmployeeRow);

    openWithExcelAction = new QAction(tr("Open with excel..."),this);
    weekTable->addAction(openWithExcelAction);
    weekTable->setContextMenuPolicy(Qt::ActionsContextMenu);
    connect(openWithExcelAction, &QAction::triggered, this, &QEmployeeShiftsWeeklyReport::openWithExcel);

    branchFullName.insert("BR2", tr("Kalampakas 104"));
    branchFullName.insert("BR3",tr("Pylis 39"));
    branchFullName.insert("BR4",tr("Pylis 98"));
    branchFullName.insert("BR5", tr("Karditsis 54"));
    hidden_rows.clear();
}

QEmployeeShiftsWeeklyReport::~QEmployeeShiftsWeeklyReport()
{
    delete ui;
}

void QEmployeeShiftsWeeklyReport::createWeekReport(int week)
{

}

void QEmployeeShiftsWeeklyReport::createWeekReport(const QDate &date)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    weekTable->clear();
    weekTable->setRowCount(60);
    weekTable->setColumnCount(9);
    initWeekTable();
    //Update title and date
    QDate dd = date;
    wItem(0,0)->setText(tr("Shifts Program from %1 to %2")
                        .arg(date.toString("dd/MM"))
                        .arg(date.addDays(6).toString("dd/MM")));
    for(int j = 2; j< 9; ++j) {
        wItem(2,j)->setText(dd.toString("dd"));
        dd = dd.addDays(1);
        wItem(3,j)->setText(dd.toString("MMM").toUpper());
    }

    QVector<WeekReportInfo> wri_vec;
    WeekReportInfo wri;
    Algorithmos::ShiftType shift_type;
    qint64 est_col = pEST->columnForDate(date);
    for(int row = 0; row < pEST->rowCount(); ++row) {
        if(!pEST->verticalHeaderItem(row) || pEST->verticalHeaderItem(row)->text()=="") //ignore empty rows
            continue;
        wri.name = pEST->verticalHeaderItem(row)->text();
        for(int col = est_col; col < est_col + 7; ++col) {
            //TODO : IMplement me!!
            shift_type = (Algorithmos::ShiftType)pEST->item(row,col)->data(Algorithmos::STIROLE).toInt();
            if(shift_type == Algorithmos::DAYOFF) {
                wri.branches << tr("DO");
            } else if (shift_type == Algorithmos::AVAILABLE)
                wri.branches << tr("AVAIL");
            else
                wri.branches << pEST->item(row,col)->text();
            wri.shifts.push_back(shift_type);
        }
        wri_vec.push_back(wri);
        //qDebug() << wri << endl << "Total : " << wri_vec.size() << " records" << endl;
        wri.shifts.clear();
        wri.branches.clear();
    }

    //Now Fill the weekReport
    // iterate through shifts and set workingHours like this :
    // item(branchRow[wri.branches[i]],i+2)->setText(wri.workingHours(...)
    int startRow = 4;
    QMap<QString, int> branchRow;
    for(int i=0; i < wri_vec.size(); ++i){
        branchRow = getBranchLines(startRow,wri_vec[i].branches,branchRow);
        QMapIterator<QString, int> iter(branchRow);
         while (iter.hasNext()) {
             iter.next();
             wItem(iter.value(),0)->setText(wri_vec[i].name); //set the employee name
             wItem(iter.value(),1)->setText(branchFullName.value(iter.key())); //set the branch full name
             wItem(iter.value(),1)->setData(Qt::UserRole,iter.key()); //set branch ID for data. Usefull when the
             //user changes the branch combo. We just iterate column 1 and hide all others branch id's except for
             // the choosen one.
         }
         dd = date;
         for(int j = 0; j < wri_vec[i].shifts.size(); ++j) {
             shift_type = (Algorithmos::ShiftType)wri_vec[i].shifts[j];
             int row;
             if(wri_vec[i].branches[j] == tr("DO") || wri_vec[i].branches[j] == tr("AVAIL"))
                 row = branchRow.first();
             else
                 row = branchRow.value(wri_vec[i].branches[j]);
             QString brCode = wri_vec[i].branches[j];
             wItem(row,j+2)->setText(wri_vec[i].workingHours(shift_type,
                                                 brCode,
                                                 j+1,
                                                 dd.month()));
             dd = dd.addDays(1);
         }
    }
    weekTable->resizeColumnsToContents();
    weekTable->resizeRowsToContents();
    QApplication::restoreOverrideCursor();
}

void QEmployeeShiftsWeeklyReport::updateWeekReport(const QString &brName)
{

}

void QEmployeeShiftsWeeklyReport::updateWeekReport(int branch)
{

}

QAbstractItemModel *QEmployeeShiftsWeeklyReport::tableModel() const
{
    return weekTable->model();
}

void QEmployeeShiftsWeeklyReport::branchChanged(const QString &branchID)
{
    if(branchID == "ALL") {
        restoreRows();
        return;
    }

    if (!hidden_rows.empty())
        restoreRows();

    const int br_col = 1;
    hidden_rows.clear();
    for(int r = 5; r < weekTable->rowCount(); ++r) {
        if(wItem(r,br_col)->data(Qt::UserRole).toString() != branchID)
            hidden_rows.push_back(r);
    }
    hideRows();

    qDebug() << "QEmployeeShiftsWeeklyReport::branchChanged(\"" << branchID << "\" )" << endl;
}

void QEmployeeShiftsWeeklyReport::print()
{
    const int ItemSize = 256;
#if !defined(QT_NO_PRINTER) && !defined(QT_NO_PRINTDIALOG)
    auto model = weekTable->model();
    auto view = weekTable;
    if (model->rowCount(QModelIndex())*model->columnCount(QModelIndex()) > 90000) {
        QMessageBox::StandardButton answer;
        answer = QMessageBox::question(this, tr("Large Image Size"),
                                       tr("The printed image may be very large. Are you sure that "
                                          "you want to print it?"),
                                       QMessageBox::Yes | QMessageBox::No);
        if (answer == QMessageBox::No)
            return;
    }

    QPrinter printer(QPrinter::ScreenResolution);

    QPrintPreviewDialog dlg(&printer, this);
    dlg.setWindowTitle(tr("Print Week Report"));
    connect(&dlg, &QPrintPreviewDialog::paintRequested, this, &QEmployeeShiftsWeeklyReport::printTable);
    if (dlg.exec() != QDialog::Accepted) {
        return;
    }


#else
    QMessageBox::information(this, tr("Printing canceled"),
                             tr("Printing is not supported on this Qt build"), QMessageBox::Cancel);
#endif
}

void QEmployeeShiftsWeeklyReport::printTable(QPrinter *printer)
{
#ifndef QT_NO_PRINTER
    weekTable->resize(printer->width(), printer->height());
    weekTable->render(printer);
#endif
}

void QEmployeeShiftsWeeklyReport::insertEmployeeRow()
{
    weekTable->insertRow(verticalHeaderView->getClickedRow() + 1);
}

void QEmployeeShiftsWeeklyReport::removeEmployeeRow()
{
    weekTable->removeRow(verticalHeaderView->getClickedRow());
}

void QEmployeeShiftsWeeklyReport::writeCSVFile(const QString &fileName)
{
    QString result = "sep=,\n";
    QTableWidgetItem *curItem = nullptr;
    QString line = "";
    for(int i = 0; i < 5; ++i) {
        line = "";
        for(int j = 0; j < weekTable->columnCount(); ++j) {
            if(!wItem(i,j))
                line += ",";
            else line += wItem(i,j)->text() + ",";
        }
        line.remove(line.size()-1,1); //Remove last comma
        result += line + "\n";
    }

    for(int i = 5; i < weekTable->rowCount(); ++i) {
        if(!wItem(i,0)) continue;
        if(wItem(i,0)->text()=="") continue;
        line = "";
        for(int j = 0; j < weekTable->columnCount(); ++j) {
            curItem = wItem(i,j);
            if(curItem)
                line += curItem->text().simplified() + ",";
        }
        line.remove(line.size()-1,1); //Remove last comma
        result += line + "\n";
    }

    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Error writing file :" << file.fileName() << endl;
    }
    QTextStream *ts = new QTextStream(&file);
    ts->setCodec("Windows-1253");
    *ts << result << "\n";
    ts->flush();
    file.close();
}

void QEmployeeShiftsWeeklyReport::openWithExcel()
{
    QDir path;
    path = QDir(path.currentPath());
    cvsFilename = path.absolutePath() + "/" + cvsFilename + ".csv";
    qDebug() << "cvsFilename : " << cvsFilename << endl;
    writeCSVFile(cvsFilename);
#ifdef Q_OS_WIN
    ShellExecute(0,0,(const wchar_t*)cvsFilename.utf16(),0,0,SW_SHOW);
#else
    QProcess process;
    QString command = "libreoffice";
    QStringList args;
    args << "--calc" << cvsFilename;
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start(command,args);
    connect(&process,
    static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
    [&process,this](int exitCode, QProcess::ExitStatus exitStatus){
        QString message = tr("Process %1 ").arg(process.program()) + tr(" finished with code : %2").arg(exitCode);
        this->setToolTip(message);
         });
    connect(&process,
            static_cast<void (QProcess::*)(QProcess::ProcessError)>(&QProcess::errorOccurred),
            [&process,this](QProcess::ProcessError error) {
        QString message;
        if(error == QProcess::FailedToStart) {
            message = tr("Program %1 not found or it is not in your path!\n Check your installation!!")
                    .arg(process.program());
            this->setToolTip(message);
        }
    });
process.waitForFinished();
#endif

}

QTableWidgetItem *QEmployeeShiftsWeeklyReport::wItem(int row, int col)
{
    return weekTable->item(row,col);
}

QMap<QString, int> &QEmployeeShiftsWeeklyReport::getBranchLines(int &sr,
                                                                const QStringList &branches,
                                                                QMap<QString, int> &bl)
{
    QStringList lst = branches;

    qDebug() <<"getBranchLines branches : "  << lst << endl;
    QString s1 = QApplication::translate("QObject","DO");
    QString s2 = QApplication::translate("QEmployeeShiftsWeeklyReport", "AVAIL");
    bl.clear();
    lst.removeAll(s1);
    lst.removeAll(s2);
    std::sort(lst.begin(), lst.end());
    auto iter = std::unique(lst.begin(), lst.end());
    lst.erase(iter, lst.end());
    for(auto it : lst) {
        bl.insert(it,++sr);
    }
    return bl;
}

void QEmployeeShiftsWeeklyReport::initWeekTable()
{

    ui->weekReportTableW->setShowGrid(true);
    //ui->weekReportTableW->resizeColumnsToContents();
    //weekTable->resizeRowsToContents();

    ui->weekReportTableW->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
//    weekTable->horizontalHeader()->hide();
//    weekTable->verticalHeader()->hide();
    for (int c = 0; c < weekTable->columnCount(); ++c) {
        QString character(QChar('A' + c));
        weekTable->setHorizontalHeaderItem(c, new QTableWidgetItem(character));
    }
    //weekTable->resizeColumnToContents(1);

    ui->weekReportTableW->setSpan(0,0,1,9);

    QFont font;
    QTableWidgetItem *title = new QTableWidgetItem();
    title->setText(tr("Shifts program from 27/03 to 04/04"));
    title->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    font = title->font();
    font.setBold(true);
    title->setFont(font);
    ui->weekReportTableW->setItem(0,0,title);

    ui->weekReportTableW->setSpan(1,0,3,2);
    QTableWidgetItem *companyTitle = new QTableWidgetItem();
    companyTitle->setText(tr("%1\n%2\n%3")
                          .arg("ΑΛΙΑΓΑΣ ΑΝΩΝΥΜΗ ΕΤΑΙΡΕΙΑ ΠΕΤΡΕΛΟΕΙΔΩΝ")
                          .arg("ΚΑΙ ΑΣΦΑΛΤΙΚΩΝ ΠΡΟΙΟΝΤΩΝ ΑΦΜ 998845066")
                          .arg("Δ.Ο.Υ. ΤΡΙΚΑΛΩΝ ΑΜΕ:9301778846"));
    companyTitle->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    font = companyTitle->font();
    font.setBold(true);
    companyTitle->setFont(font);
    ui->weekReportTableW->setItem(1,0,companyTitle);

    QTableWidgetItem *name = new QTableWidgetItem();
    name->setText("Ονοματεπώνυμο");
    name->setFont(font);
    ui->weekReportTableW->setItem(4,0,name);

    QTableWidgetItem *brName = new QTableWidgetItem();
    brName->setText("Υποκατάστημα");
    brName->setFont(font);
    ui->weekReportTableW->setItem(4,1,brName);

    ui->weekReportTableW->setSpan(4,2,1,7);
    QTableWidgetItem *workHours = new QTableWidgetItem();
    workHours->setText("ΩΡΑΡΙΟ ΕΡΓΑΣΙΑΣ");
    workHours->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    workHours->setFont(font);
    ui->weekReportTableW->setItem(4,2,workHours);

    QStringList weekDays;
    weekDays << tr("MON") << tr("TUE") << tr("WED")
             << tr("THI") << tr("FRI") << tr("SAT")
             << tr("SUN");
    for(int j=2; j< 9; ++j) {
        QTableWidgetItem *it = new QTableWidgetItem();
        it->setText(weekDays[j-2]);
        it->setTextAlignment(Qt::AlignCenter);
        ui->weekReportTableW->setItem(1,j,it);
        QTableWidgetItem *it1 = new QTableWidgetItem();
        it1->setText(QString::number(j));
        it1->setTextAlignment(Qt::AlignCenter);
        ui->weekReportTableW->setItem(2,j,it1);
        QTableWidgetItem *it2 = new QTableWidgetItem();
        it2->setText(QDate::currentDate().toString("MMM").toUpper());
        it2->setTextAlignment(Qt::AlignCenter);
        ui->weekReportTableW->setItem(3,j,it2);
    }

    //Rest table. Dummy fill it
    for(int row = 5; row < weekTable->rowCount(); ++row)
        for(int col = 0; col < weekTable->columnCount(); ++col) {
            QTableWidgetItem *dummy_item = new QTableWidgetItem("");
            weekTable->setItem(row,col,dummy_item);
        }
}

void QEmployeeShiftsWeeklyReport::hideRows()
{
    for(int i = 0; i < hidden_rows.size(); ++i)
        weekTable->hideRow(hidden_rows[i]);
}

void QEmployeeShiftsWeeklyReport::restoreRows()
{
    for(int i = 0; i < hidden_rows.size(); ++i)
        weekTable->showRow(hidden_rows[i]);
}
