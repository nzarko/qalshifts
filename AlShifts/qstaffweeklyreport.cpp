#include <QFont>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QProcess>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#include "qstaffweeklyreport.h"
#include "qemployeeshiftstable.h"

QStaffWeeklyReport::QStaffWeeklyReport(QEmployeeShiftsTable *empl, QWidget *parent) :
    QTableWidget(parent),
    pEST(empl),
    weekDate(QDate::currentDate())
{
    startRow = 2;
    brColumn.insert("BR2",4);
    brColumn.insert("BR3",5);
    brColumn.insert("BR4",6);
    brColumn.insert("BR5",7);
    brColumn.insert(tr("DO"),8);
    brColumn.insert(tr("AVAIL"),8);


    for(int i = 1; i<=7; ++i) {
        WeekDaysRow wdr ={0,0};
        weekDaysRow.insert(i,wdr);
        fuelweekDaysRow.insert(i,wdr);
    }

    openWithExcelAction = new QAction(tr("Open with excel ..."));
    addAction(openWithExcelAction);
    setContextMenuPolicy(Qt::ActionsContextMenu);
    connect(openWithExcelAction, &QAction::triggered, this, &QStaffWeeklyReport::openWithExcel);
}

QStaffWeeklyReport::~QStaffWeeklyReport()
{

}

void QStaffWeeklyReport::createWeekReport(const QDate &date)
{
    weekDate = date;
    init(date);

    QMap<QString ,float> weekWorkHours;
    //First get the week report vector
    getWeekReportInfo(wriVec);
    getWeekWorkHours(wriVec, weekWorkHours);

    //Fill table with names first
    QString cBranch; //day branch
    Algorithmos::ShiftType s_type;
    int cDay; //day number;
    QString name; //Employee name
    int start_row; //day start row.
    int branch_col;
    QTableWidgetItem *pItem = nullptr;
    for(int i = 0; i <=9; ++i) {
        name = wriVec[i].name;
        for(int j=0; j<wriVec[i].shifts.size()/*7*/; ++j) {
            cBranch = wriVec[i].branches[j];
            s_type =  wriVec[i].shifts[j];
            cDay = j;
            start_row = weekDaysRow[cDay].startRow;
            branch_col = brColumn[cBranch];
            int item_row;
            //Finally create the item
            if(branch_col == 8) {
                QString item_text;
                if(s_type == Algorithmos::DAYOFF) {
                    item_text = name + "(" + tr("DO") + ")";
                    item_row = start_row;
                }
                else if (s_type == Algorithmos::AVAILABLE) {
                    item_text = name + "(" + tr("AVAIL") + ")";
                    item_row = start_row + 1;
                }
                pItem = item(item_row,8); //case DAYOFF or AVAILABLE
                if(pItem) {
                    QString it = pItem->text();
                    it.push_back(tr("\n%1").arg(item_text));
                    pItem->setText(it);
                } else {
                    pItem = new QTableWidgetItem(item_text);
                    setItem(item_row,branch_col,pItem);
                    if(s_type == Algorithmos::DAYOFF) {
                        pItem = new QTableWidgetItem(date.addDays(j).toString("dd/MM/yyyy"));
                        setItem(item_row, branch_col+1,pItem);
                    }
                }
            } else {
                pItem = new QTableWidgetItem(name);
                item_row = (s_type == Algorithmos::EARLY) ? start_row : (start_row+1);
                setItem(item_row,branch_col,pItem);
            }
        }
    }
    //Employee list
    start_row = weekDaysRow[0].startRow + 2;
    for(int i = 0; i <= 9; i++) {
        pItem = new QTableWidgetItem(wriVec[i].name);
        setItem(start_row+i, 10,pItem);
    }

    //Summary report for Managers and Fuel Managers.
    start_row = 24;
    for(int i = 0; i <= 9; ++i ) {
        pItem = new QTableWidgetItem(QString::number(i+1));
        setItem(24 + i,3, pItem); //A\A
        pItem = new QTableWidgetItem(wriVec[i].name);
        setItem(24+i, 4, pItem); //name
        pItem = new QTableWidgetItem(QString::number(weekWorkHours[wriVec[i].name]));
        setItem(24+i, 5, pItem);
        pItem = new QTableWidgetItem(date.addDays(6).toString("dd/MM/yyyy"));
        setItem(24+i,10,pItem);
    }

    //Do the same for fuel employees.
    for(int i = 9; i < wriVec.size(); ++i) {
        name = wriVec[i].name;
        for(int j=0; j<wriVec[i].shifts.size()/*7*/; ++j) {
            cBranch = wriVec[i].branches[j];
            s_type =  wriVec[i].shifts[j];
            cDay = j;
            start_row = fuelweekDaysRow[cDay].startRow;
            branch_col = brColumn[cBranch];
            int item_row;
            //Finally create the item
            if(branch_col == 8) {
                QString item_text;
                if(s_type == Algorithmos::DAYOFF) {
                    item_text = name + "(" + tr("DO") + ")";
                    item_row = start_row;
                }
                else if (s_type == Algorithmos::AVAILABLE) {
                    item_text = name + "(" + tr("AVAIL") + ")";
                    item_row = start_row + 1;
                }
                pItem = item(item_row,8); //case DAYOFF or AVAILABLE
                if(pItem) {
                    QString it = pItem->text();
                    it.push_back(tr("\n%1").arg(item_text));
                    pItem->setText(it);
                } else {
                    pItem = new QTableWidgetItem(item_text);
                    setItem(item_row,branch_col,pItem);
                    if(s_type == Algorithmos::DAYOFF) {
                        pItem = new QTableWidgetItem(date.addDays(j).toString("dd/MM/yyyy"));
                        setItem(item_row, branch_col+1,pItem);
                    }
                }
            } else {
                pItem = new QTableWidgetItem(name);
                if(s_type == Algorithmos::EARLY)
                    item_row = start_row;
                else if ( s_type == Algorithmos::INTERMITTENT)
                    item_row = start_row+1;
                else if(s_type == Algorithmos::LATE && cDay < 6)
                    item_row = start_row + 3;
                else if(s_type == Algorithmos::LATE && cDay == 6)
                    item_row = start_row + 1;
                setItem(item_row,branch_col,pItem);
                if(s_type == Algorithmos::INTERMITTENT) {
                    QTableWidgetItem *clone = new QTableWidgetItem(*pItem);
                    setItem(item_row+1,branch_col, clone);
                }
            }
        }
    }
    //Employee list
    start_row = fuelweekDaysRow[0].startRow + 2;
    for(int i = 9; i < wriVec.size(); i++) {
        pItem = new QTableWidgetItem(wriVec[i].name);
        setItem(start_row+i - 8, 10,pItem);
    }

    //Summary report for Managers and Fuel Managers.
    start_row = 72;
    for(int i = 9; i < wriVec.size(); ++i ) {
        pItem = new QTableWidgetItem(QString::number(i-8));
        setItem(start_row + i - 8,3, pItem); //A\A
        pItem = new QTableWidgetItem(wriVec[i].name);
        setItem(start_row+i - 8, 4, pItem); //name
        pItem = new QTableWidgetItem(QString::number(weekWorkHours[wriVec[i].name]));
        setItem(start_row+i-8, 5, pItem);
        pItem = new QTableWidgetItem(date.addDays(6).toString("dd/MM/yyyy"));
        setItem(start_row+i-8,10,pItem);
    }

    resizeColumnsToContents();
    resizeRowsToContents();
    //refineRows();

}

void QStaffWeeklyReport::setCSVFilename(const QString &filename)
{
    csvFilename = filename;
}

void QStaffWeeklyReport::init(const QDate &date)
{
    setRowCount(0);
    setColumnCount(0);

    setRowCount(105);
    setColumnCount(15);

    for (int c = 0; c < columnCount(); ++c) {
        QString character(QChar('A' + c));
        setHorizontalHeaderItem(c, new QTableWidgetItem(character));
    }
    startRow = 2;
    createSkeleton(weekDate);
    createSkeleton(weekDate,true);
}

void QStaffWeeklyReport::createSkeleton(const QDate &date, bool forFuelEmployees)
{
    //Skeleton
    QFont serifFont("Times", 10, QFont::Bold);

    QStringList lst;
    lst << "ΗΜΕΡΑ" << "ΩΡΑΡΙΟ" << "ΩΡΕΣ" ;
    setSpan(startRow - 2,0,2,1);
    setSpan(startRow - 2,1,2,2);

    for(int j = 0; j < 2; ++j) {
        QTableWidgetItem *pItem = new QTableWidgetItem(lst[j]);
        pItem->setFont(serifFont);
        pItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        setItem(startRow-2,j,pItem);
    }
    setSpan(startRow - 2,3,2,1);
    QTableWidgetItem *pItem = new QTableWidgetItem(lst[2]);
    pItem->setFont(serifFont);
    pItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    setItem(startRow - 2,3,pItem);

    setSpan(startRow-2,4,1,6);
    QString title = tr("%1η Εβδομάδα (%2 έως %3)")
            .arg(date.weekNumber())
            .arg(date.toString("dd-MM-yyyy"))
            .arg(date.addDays(6).toString("dd-MM-yyyy"));
    pItem = new QTableWidgetItem();
    pItem->setText(title);
    pItem->setFont(serifFont);
    pItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    setItem(startRow - 2,4,pItem);

    lst.clear();
    lst << "ΠΡΑΤΗΡΙΟ\nΚΑΛΑΜΠΑΚΑΣ"
        << "ΠΡΑΤΗΡΙΟ\nΑΓ. ΜΟΝΗΣ" << "ΠΡΑΤΗΡΙΟ\nΠΥΛΗΣ 98"
        <<"ΠΡΑΤΗΡΙΟ\nΚΑΡΔΙΤΣΗΣ 54"
       <<"ΡΕΠΟ";
    for(int j = 4; j<=8; ++j) {
        pItem = new QTableWidgetItem(lst[j-4]);
        pItem->setFont(serifFont);
        pItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        setItem(startRow-1,j,pItem);
    }
    resizeRowToContents(startRow-1);

    lst.clear();
    lst << "ΔΕΥΤΕΡΑ" << "ΤΡΙΤΗ" << "ΤΕΤΑΡΤΗ"
        <<"ΠΕΜΠΤΗ" << "ΠΑΡΑΣΚΕΥΗ" <<"ΣΑΒΒΑΤΟ"
       <<"ΚΥΡΙΑΚΗ";
    QStringList hour_list, weekend_hour_list;
    int span = 1;
    if(forFuelEmployees) {
    hour_list << "06:30" << "14:30" << "8.0"
              << "08:00" << "13:00" << "5.0"
              << "17:00" << "20:00" << "3.0"
              << "14:30" << "22:00" << "7.5";
    weekend_hour_list << "07:00" << "14:30" << "7.5"
                      << "08:00" << "13:00" << "5.0"
                      << "17:00" << "20:00" << "3.0"
                        << "14:30" << "21:00" << "7.5"
                        <<"07:00" << "14:00" << "7.0"
                       <<"14:00" << "21:00" <<"7.0";
    span = 4;
    } else {
        hour_list << "06:30" << "14:30" << "8.0"
                  << "14:30" << "22:00" << "7.5";
        weekend_hour_list << "07:00" << "14:30" << "7.5"
                            << "14:30" << "21:00" << "7.5"
                            <<"07:00" << "14:00" << "7.0"
                           <<"14:00" << "21:00" <<"7.0";
        span = 2;
    }
    int k = 0;
    int r = startRow;
    while (k < lst.size()-2) {
        setSpan(r,0,span,1);
        pItem = new QTableWidgetItem(lst[k]);
        pItem->setFont(serifFont);
        pItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        setItem(r,0,pItem);
        if(!forFuelEmployees)
            weekDaysRow[k] = {r,rowSpan(r,0)};
        else
            fuelweekDaysRow[k] = {r,rowSpan(r,0)};
        int l = 0;
        for(int i = r; i < r + rowSpan(r,0); ++i)
            for(int j=1; j<= 3; ++j) {
                pItem = new QTableWidgetItem(hour_list[l]);
                pItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
                setItem(i,j,pItem);
                ++l;
            }
        ++k;
        r += rowSpan(r,0) + 1;
    }
    //Special Treatment for the weekend.
    //k-=1;
    while (k < lst.size()) {
        if(k==6) span = 2;
        setSpan(r,0,span,1);
        pItem = new QTableWidgetItem(lst[k]);
        pItem->setFont(serifFont);
        pItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        setItem(r,0,pItem);
        if(!forFuelEmployees)
            weekDaysRow[k] = {r,rowSpan(r,0)};
        else
            fuelweekDaysRow[k] = {r,rowSpan(r,0)};
        int l = 0;
        for(int i = r; i < r + rowSpan(r,0); ++i)
            for(int j=1; j<= 3; ++j) {
                if(k == 6) {
                    weekend_hour_list.clear();
                    weekend_hour_list <<"07:00" << "14:00" << "7.0"
                                     <<"14:00" << "21:00" <<"7.0";
                }
                pItem = new QTableWidgetItem(weekend_hour_list[l]);
                pItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
                setItem(i,j,pItem);
                ++l;
            }
        ++k;
        r += rowSpan(r,0) + 1;
    }
    resizeColumnToContents(1);
    resizeColumnToContents(2);
    resizeColumnToContents(3);
    resizeColumnToContents(4);

    lst.clear();
    lst << "Α/Α" << "ΟΝΟΜΑ" << "ΩΡΕΣ" << " " << " "
        << "ΠΑΡΑΤΗΡΗΣΕΙΣ"
        << "Ημερ. Ρεπό\nΤελ. Κυριακής"
        << "Ημερ. Τελ.\nΚυριακής"
        << "Διαφορά";

    int j = 3;
    while(j - 3 < lst.size()) {
        pItem = new QTableWidgetItem(lst[j-3]);
        pItem->setFont(serifFont);
        pItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        setItem(r,j,pItem);
        ++j;
    }
    resizeRowToContents(r);
    startRow = r + 15;
}

void QStaffWeeklyReport::getWeekReportInfo(QVector<WeekReportInfo> &wri_vec)
{
    wri_vec.clear();
    WeekReportInfo wri;
    Algorithmos::ShiftType shift_type;
    qint64 est_col = pEST->columnForDate(weekDate);
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
    qDebug() << "Weekreportinfo 10th name : " << wri_vec[10].name << endl;
}

float QStaffWeeklyReport::dayWorkHours(int day, Algorithmos::ShiftType shiftType)
{
    float hours = 0;
    switch (day) {
    case 1: case 2: case 3: case 4: case  5:
        if(shiftType == Algorithmos::EARLY || shiftType == Algorithmos::INTERMITTENT)
            hours = 8.0;
        else if(shiftType == Algorithmos::LATE)
            hours = 7.5;
        break;
    case 6:
        if(shiftType == Algorithmos::EARLY || shiftType == Algorithmos::LATE)
            hours = 7.5;
        else if(shiftType == Algorithmos::INTERMITTENT)
            hours = 8.0;
        break;
    case 7:
        if(shiftType == Algorithmos::EARLY || shiftType == Algorithmos::LATE)
            hours = 7.0;
        break;
    }
    return hours;
}

void QStaffWeeklyReport::getWeekWorkHours(QVector<WeekReportInfo> &wri_vec, QMap<QString, float> &weekWH)
{
    weekWH.clear();
    //Initialize first
    for(int i = 0; i < wri_vec.size(); ++i)
        weekWH.insert(wri_vec[i].name, 0.0);

    //Now do the real job
    Algorithmos::ShiftType stype;
    for(int i = 0; i < wri_vec.size(); ++i) {
        for(int j = 0; j < wri_vec[i].shifts.size(); ++j) {
            stype = wri_vec[i].shifts[j];
            if(stype == Algorithmos::DAYOFF || stype == Algorithmos::AVAILABLE)
                continue;
            weekWH[wri_vec[i].name] += dayWorkHours(j+1,stype);
        }
    }

    //qDebug() << weekWH <<endl;
}

void QStaffWeeklyReport::refineRows()
{
    for(int i = 0; i < rowCount(); ++i) {
        if (!item(i,4))
            setRowHeight(i,5);
    }
}

bool QStaffWeeklyReport::writeCSVFile(const QString &filename)
{
    QString result = "sep=,\n";
    QTableWidgetItem *curItem = nullptr;
    QString line = "";

    for(int i = 0; i < rowCount(); ++i) {
        line = "";
        for(int j = 0; j < columnCount(); ++j) {
            curItem = item(i,j);
            if(curItem)
                line += curItem->text().simplified() + ",";
            else line.append(",");
        }
        line.remove(line.size()-1,1); //Remove last comma
        result += line + "\n";
    }

    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Error writing file :" << file.fileName() << endl;
        return false;
    }
    QTextStream ts(&file);
    ts << result << "\n";
    file.close();
    return true;
}

void QStaffWeeklyReport::openWithExcel()
{
    QDir path;
    path = QDir(path.currentPath());
    csvFilename = path.absolutePath() + "/" + csvFilename + ".csv";
    qDebug() << "cvsFilename : " << csvFilename << endl;
    writeCSVFile(csvFilename);
#ifdef Q_OS_WIN
    ShellExecute(0,0,(const wchar_t*)cvsFilename.utf16(),0,0,SW_SHOW);
#else
    QProcess process;
    QString command = "libreoffice";
    QStringList args;
    args << "--calc" << csvFilename;
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
