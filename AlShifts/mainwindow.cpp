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
#include <QLabel>
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
    setCurrentFile("");
}

MainWindow::~MainWindow()
{
    delete m_centralView;
    delete dateSelectorDlg;
    delete ui;
}

void MainWindow::setupActions()
{

    connect(ui->actionNew,&QAction::triggered, this, &MainWindow::newFile);
    connect(ui->actionOpen, &QAction::triggered, this,
            &MainWindow::open);
    connect(ui->actionSave, &QAction::triggered,this,
            &MainWindow::save);
    connect(ui->actionSave_As, &QAction::triggered, this,
            &MainWindow::saveAs);
    ui->action_Populate->setEnabled(m_centralView->employeeShiftsTable()->isEmpty());
    connect(ui->action_Populate,&QAction::triggered, m_centralView, &CentralView::populate);
    connect(m_centralView->employeeShiftsTable(),&QEmployeeShiftsTable::populationChanged,ui->action_Populate,
            &QAction::setDisabled);
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
    connect(ui->actionPrepare, &QAction::triggered,
             m_centralView->employeeShiftsTable(), &QEmployeeShiftsTable::loadBFuelShifts);

    /* ************************************ *
          Recent File Menu
    * *********************************** */
    recentFilesMenu = new QMenu(tr("Recent Files ..."),ui->menu_File);
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
    locationLabel = new QLabel(" W999 " );
    locationLabel->setAlignment(Qt::AlignCenter);
    locationLabel->setMinimumSize(locationLabel->sizeHint());
    statusBar()->addWidget(locationLabel);
    connect(esht, &QEmployeeShiftsTable::currentCellChanged,this,
            &MainWindow::updateStatusBar);
    connect(esht, &QEmployeeShiftsTable::modified,
            this, &MainWindow::documentWasModified);
    updateStatusBar();
}

void MainWindow::readSettings()
{
    QSettings settings("Algorithmos", "QALShifts");

    recentFiles = settings.value("recentFiles").toStringList();
    updateRecentFileActions();

    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(800, 600)).toSize();
    restoreState(settings.value("windowState").toByteArray());
    resize(size);
    move(pos);
}

void MainWindow::writeSettings()
{
    QSettings settings("Algorithmos", "QALShifts");
     settings.setValue("recentFiles", recentFiles);
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("windowState", saveState());
    //Config().read();
}

bool MainWindow::maybeSave()
{
    if (isWindowModified()) {
        int r = QMessageBox::warning(this, tr("Aliagas Shifts"),
                        tr("The document has been modified.\n"
                           "Do you want to save your changes?"),
                        QMessageBox::Yes | QMessageBox::Default,
                        QMessageBox::No,
                        QMessageBox::Cancel | QMessageBox::Escape);
        if (r == QMessageBox::Yes) {
            return save();
        } else if (r == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

bool MainWindow::loadFile(const QString &fileName)
{
    QEmployeeShiftsTable *spreadsheet = m_centralView->employeeShiftsTable();
    if (!spreadsheet->readFile(fileName)) {
        statusBar()->showMessage(tr("Loading canceled"), 2000);
        return false;
    }

    setCurrentFile(fileName);
    updateRecentFiles(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
    return true;
}

bool MainWindow::saveFile(const QString &fileName)
{
    QEmployeeShiftsTable *spreadsheet = m_centralView->employeeShiftsTable();
    if (!spreadsheet->writeFile(fileName)) {
        statusBar()->showMessage(tr("Saving canceled"), 2000);
        return false;
    }

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::updateRecentFileActions()
{
    QSettings settings("Algorithmos", "QALShifts");
    recentFiles = settings.value("recentFiles").toStringList();
    QMutableStringListIterator i(recentFiles);
    while (i.hasNext()) {
        if (!QFile::exists(i.next()))
            i.remove();
    }

    for (int j = 0; j < MaxRecentFiles; ++j) {
        if (j < recentFiles.count()) {
            QString text = tr("&%1 %2")
                           .arg(j + 1)
                           .arg(strippedName(recentFiles[j]));
            recentFileActions[j]->setText(text);
            recentFileActions[j]->setData(recentFiles[j]);
            recentFileActions[j]->setVisible(true);
        } else {
            recentFileActions[j]->setVisible(false);
        }
    }
}

void MainWindow::updateRecentFiles(const QString &fileName)
{
    QSettings settings("Algorithmos", "QALShifts");
    recentFiles = settings.value("recentFiles").toStringList();
    recentFiles.removeAll(fileName);
    recentFiles.prepend(fileName);
    while (recentFiles.size() > MaxRecentFiles)
        recentFiles.removeLast();

    settings.setValue("recentFiles", recentFiles);
    updateRecentFileActions();
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
    QEmployeeShiftsTable *spreadsheet = m_centralView->employeeShiftsTable();
    if (maybeSave()) {
        spreadsheet->clear();
        selectStartDate();
        setCurrentFile("");
    }
    //updateRecentFiles();
}

void MainWindow::open()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this,
                                   tr("Open Aliagas Shifts"), ".",
                                   tr("Aliagas Shifts files (*.alsh)"));
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

bool MainWindow::save()
{
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                               tr("Save Shifts"), ".",
                               tr("Aliagas Shifts files (*.alsh)"));
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
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
    setWindowModified(true);
    updateStatusBar();
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
    curFile = fileName;
    setWindowModified(false);

    QString shownName = "Untitled";
    if (!curFile.isEmpty()) {
        shownName = strippedName(curFile);
        recentFiles.removeAll(curFile);
        recentFiles.prepend(curFile);
        updateRecentFileActions();
    }

    setWindowTitle(tr("%1[*] - %2").arg(shownName)
                                   .arg(tr("Aliagas Shifts")));
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
    locationLabel->setText(m_centralView->employeeShiftsTable()->currentLocation());
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
