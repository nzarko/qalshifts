#include <QCalendarWidget>
#include <QDebug>
#include <QMenu>
#include <QAction>
#include <QStatusBar>
#include <QApplication>
#include <QMessageBox>
#include <QSettings>
#include <QByteArray>
#include <QList>
#include <QCloseEvent>
#include <QFileInfo>
#include <QFileDialog>
#include <QPrintDialog>
#include <QPrintPreviewDialog>

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
    createStatusBar();

    //setWindowState(Qt::WindowMaximized);
    readSettings();
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
    m_centralView->employeeShiftsTable()->addAction(ui->actionRearrange_Employees_Shift);
    m_centralView->employeeShiftsTable()->setContextMenuPolicy(Qt::ActionsContextMenu);
    connect(ui->actionSwap_Shifts, SIGNAL(triggered()),
            m_centralView->employeeShiftsTable(), SLOT(swapShifts()));
    connect(ui->actionRearrange_Employees_Shift, &QAction::triggered,
            m_centralView->employeeShiftsTable(), &QEmployeeShiftsTable::rearrangeEmployeesShift);

    /* ************************************ *
          Recent File Menu
    * *********************************** */
    recentFilesMenu = new QMenu(ui->menu_File);
    ui->menu_File->insertAction(ui->actionprint,recentFilesMenu->menuAction());
    for(int i = 0 ; i < MaxRecentFiles; i++)
    {
        recentFileActions[i] = new QAction(this);
        recentFileActions[i]->setVisible(false);
        connect(recentFileActions[i], SIGNAL(triggered()), this,
                SLOT(openRecentFile()));
        recentFilesMenu->addAction(recentFileActions[i]);
    }
    updateRecentFileActions();

}

void MainWindow::createStatusBar()
{
    QEmployeeShiftsTable *esht = m_centralView->employeeShiftsTable();
    connect(esht, &QEmployeeShiftsTable::currentCellChanged,this,
            &MainWindow::updateStatusBar);
}

void MainWindow::readSettings()
{
    QSettings settings("Algorithmos", "QALShifts");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(800, 600)).toSize();
    restoreState(settings.value("windowState").toByteArray());
    resize(size);
    move(pos);
}

void MainWindow::writeSettings()
{
    QSettings settings("Algorithmos", "QALShifts");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("windowState", saveState());
    //Config().read();
}

bool MainWindow::maybeSave()
{
    return true;
}

void MainWindow::loadFile(const QString &fileName)
{

}

bool MainWindow::saveFile(const QString &fileName)
{
    return false;
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QString();
}

void MainWindow::updateRecentFileActions()
{

}

void MainWindow::updateRecentFiles(const QString &)
{

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

void MainWindow::newFile()
{

}

void MainWindow::open()
{

}

bool MainWindow::save()
{
    return false;
}

bool MainWindow::saveAs()
{
    return false;
}

bool MainWindow::saveAll()
{
    return false;
}

void MainWindow::about()
{

}

void MainWindow::documentWasModified()
{

}

void MainWindow::openRecentFile()
{
    if (maybeSave()) {
        QAction *action = qobject_cast<QAction*>(sender());
        if(action) {
            loadFile(action->data().toString());
        }
    }
}

void MainWindow::setCurrentFile(const QString &fileName)
{

}

void MainWindow::fileprint()
{

}

void MainWindow::filePrintPreview()
{

}

void MainWindow::printPreview(QPrinter *printer)
{

}

void MainWindow::showPreferencesDialog()
{

}

void MainWindow::updateStatusBar()
{

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        saveAll();
        writeSettings();
        //Config().write();
        event->accept();
    } else {
        event->ignore();
    }
}
