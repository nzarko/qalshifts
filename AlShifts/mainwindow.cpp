#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "centralview.h"
#include "qemployeeshiftstable.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
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
    delete ui;
}

void MainWindow::setupActions()
{
    ui->action_Populate->setEnabled(m_centralView->employeeShiftsTable()->isEmpty());
    connect(ui->action_Populate,SIGNAL(triggered()), m_centralView, SLOT(populate()));
    connect(m_centralView->employeeShiftsTable(),SIGNAL(populationChanged(bool)),ui->action_Populate,
            SLOT(setDisabled(bool)));
    connect(ui->actionClear_Shifts,SIGNAL(triggered()),m_centralView->employeeShiftsTable(), SLOT(clearShifts()));
}
