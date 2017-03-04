#include <QCalendarWidget>
#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "centralview.h"
#include "qemployeeshiftstable.h"
#include "qdateselector.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    dateSelectorDlg(0)
{
    ui->setupUi(this);
    move(500,100);
    m_centralView = new CentralView();
    setCentralWidget(m_centralView);
    setupActions();

}

MainWindow::~MainWindow()
{
    delete m_centralView;
    delete dateSelectorDlg;
    delete ui;
}

void MainWindow::setupActions()
{

    connect(ui->actionNew,SIGNAL(triggered()), this, SLOT(selectStartDate()));
    ui->action_Populate->setEnabled(m_centralView->employeeShiftsTable()->isEmpty());
    connect(ui->action_Populate,SIGNAL(triggered()), m_centralView, SLOT(populate()));
    connect(m_centralView->employeeShiftsTable(),SIGNAL(populationChanged(bool)),ui->action_Populate,
            SLOT(setDisabled(bool)));
    ui->actionSolve->setEnabled(false);
    connect(m_centralView->employeeShiftsTable(),SIGNAL(populationChanged(bool)),ui->actionSolve,
            SLOT(setEnabled(bool)));
    connect(ui->actionClear_Shifts,SIGNAL(triggered()),m_centralView->employeeShiftsTable(), SLOT(clearShifts()));
    connect(ui->actionprint,SIGNAL(triggered()), m_centralView->employeeShiftsTable(),SLOT(print()));
    connect(ui->actionPrint_Preview,SIGNAL(triggered()), m_centralView->employeeShiftsTable(), SLOT(printPreview()));
    connect(ui->actionHtml,SIGNAL(triggered()),m_centralView->employeeShiftsTable(), SLOT(exportToHtml()));
    connect(ui->actionSolve, SIGNAL(triggered()), m_centralView->employeeShiftsTable(), SLOT(solve()));

    m_centralView->employeeShiftsTable()->addAction(ui->actionSwap_Shifts);
    m_centralView->employeeShiftsTable()->setContextMenuPolicy(Qt::ActionsContextMenu);
    connect(ui->actionSwap_Shifts, SIGNAL(triggered()),
            m_centralView->employeeShiftsTable(), SLOT(swapShifts()));
}

void MainWindow::selectStartDate()
{
    ///TODO : Implement me!!
    if(!dateSelectorDlg)
        dateSelectorDlg = new QDateSelector();
    int result = dateSelectorDlg->exec();
    if(result == QDialog::Accepted) {
        m_startDate = dateSelectorDlg->selectedDate();
        this->m_centralView->employeeShiftsTable()->setStartDate(QDateTime(m_startDate));
        this->m_centralView->employeeShiftsTable()->populate();
        qDebug() << "Selected date : " << m_startDate.toString("ddd dd MMM yyyy");
    }
}
