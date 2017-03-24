#include "qemployeeshiftsweeklyreport.h"
#include "ui_qemployeeshiftsweeklyreport.h"

QEmployeeShiftsWeeklyReport::QEmployeeShiftsWeeklyReport(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QEmployeeShiftsWeeklyReport)
{
    ui->setupUi(this);

    ui->weekReportTableW->setSpan(0,0,1,7);

    QTableWidgetItem *title = new QTableWidgetItem();
    title->setText(tr("Shifts programm from 27/03 to 04/04"));
    title->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->weekReportTableW->setItem(0,0,title);
}

QEmployeeShiftsWeeklyReport::~QEmployeeShiftsWeeklyReport()
{
    delete ui;
}
