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
#include <QToolBar>
#include <QComboBox>

#include <QPrintDialog>
#include <QPrinter>
#include <QPrintPreviewDialog>
#include <QProgressDialog>
#include <QPainter>


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "centralview.h"
#include "qemployeeshiftstable.h"
#include "qemployeeshiftsweeklyreport.h"
#include "qstaffweeklyreport.h"
#include "qdateselector.h"
#include "printview.h"
#include "alshiftssettingsdialog.h"
#include "aboutdialog.h"


MainWindow* MainWindow::m_pInstance = nullptr;

MainWindow* MainWindow::instance()
{
    if ( !m_pInstance )
        m_pInstance = new MainWindow;

    return m_pInstance;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_startDate(QDate::currentDate()),
    weekViewToolBar(0),
    weekCB(0),
    branchCB(0),
    dateSelectorDlg(0),
    settingsDlg(0)
{
    ui->setupUi(this);
    move(500,100);
    m_centralView = new CentralView();
    setCentralWidget(m_centralView);    
    //Fill up branch Combo box

    branches.clear();
    branches.insert("ALL",tr("All"));
    branches.insert("BR2", tr("Kalampakas 104"));
    branches.insert("BR3",tr("Pylis 39"));
    branches.insert("BR4",tr("Pylis 98"));
    branches.insert("BR5",tr("Karditsis 54"));

    setupActions();
    createStatusBar();    

    //setWindowState(Qt::WindowMaximized);
    readSettings();
    createToolBar();
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
    connect(ui->actionExit, &QAction::triggered, qApp, &QApplication::closeAllWindows);
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
    connect(ui->actionprint,&QAction::triggered, this, &MainWindow::fileprint);
    connect(ui->actionPrint_Preview,SIGNAL(triggered()), m_centralView->employeeShiftsTable(), SLOT(printPreview()));
    connect(ui->actionHtml,SIGNAL(triggered()),m_centralView->employeeShiftsTable(), SLOT(exportToHtml()));
    connect(ui->actionSolve, SIGNAL(triggered()), m_centralView->employeeShiftsTable(), SLOT(solve()));

    m_centralView->employeeShiftsTable()->addAction(ui->actionSwap_Shifts);
    m_centralView->employeeShiftsTable()->addAction(ui->actionRearrange_Employees_Shift);
    m_centralView->employeeShiftsTable()->addAction(ui->actionForce_Intermittent);
    m_centralView->employeeShiftsTable()->setContextMenuPolicy(Qt::ActionsContextMenu);
    connect(ui->actionSwap_Shifts, SIGNAL(triggered()),
            m_centralView->employeeShiftsTable(), SLOT(swapShifts()));
    connect(ui->actionRearrange_Employees_Shift, &QAction::triggered,
            m_centralView->employeeShiftsTable(), &QEmployeeShiftsTable::rearrangeEmployeesShift);
    connect(ui->actionPrepare, &QAction::triggered,
             m_centralView->employeeShiftsTable(), &QEmployeeShiftsTable::loadBFuelShifts);
    connect(ui->actionForce_Intermittent, &QAction::triggered, m_centralView->employeeShiftsTable(),
            &QEmployeeShiftsTable::forceIntermittent);

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

    /* *********************************** *
     *      View Menu                      *
     * *********************************** */
    connect(ui->actionWeekly, &QAction::toggled,this,&MainWindow::changeView);
    connect(ui->actionStuff_Week, &QAction::toggled, this, &MainWindow::toggleStaffWeekView);
    ui->actionBranches_Full_Name->setChecked(false);
    ui->actionBranches_Full_Name->setEnabled(false);
    connect(ui->actionBranches_Full_Name,&QAction::toggled,this->m_centralView->employeeShiftsTable(),
            &QEmployeeShiftsTable::showBranchFullNames);

    /* *********************************** *
     *      Settings Dialog                *
     * *********************************** */
    connect(ui->actionSettings, &QAction::triggered, this, &MainWindow::showPreferencesDialog);

    /* *********************************** *
     *      Help Menu                      *
     * *********************************** */
    connect(ui->actionAbout_Qt, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
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

void MainWindow::createToolBar()
{

    if(!weekViewToolBar) {
        weekViewToolBar = addToolBar(tr("Week report"));
        weekViewToolBar->setObjectName("weekViewToolBar");
    }

    if(!weekCB)
        weekCB = new QComboBox(weekViewToolBar);
    weekCB->clear();
    QString lst;
    QDate cdate = m_startDate;
    QDate ddate = m_startDate.addDays(6);
    QString format = "dd/MM/yyyy";
    for(auto i = 1; i <= 7; ++i) {
        lst = tr("Week %1 ( %2 - %3 )").arg(i)
               .arg(cdate.toString(format))
               .arg(ddate.toString(format));
        weekCB->addItem(lst,cdate);
        cdate = ddate.addDays(1);
        ddate = cdate.addDays(6);
    }
    weekCB->setCurrentIndex(0);

    if(!branchCB)
        branchCB = new QComboBox(weekViewToolBar);

    branchCB->clear();
    QMapIterator<QString, QString> iter(branches);
    while(iter.hasNext()) {
        iter.next();
        branchCB->addItem(iter.value(),iter.key());
    }
    branchCB->setCurrentIndex(0);

    weekViewToolBar->addWidget(weekCB);
    weekViewToolBar->addSeparator();
    weekViewToolBar->addWidget(branchCB);
    weekViewToolBar->setVisible(false);
    connect(weekCB, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          this,&MainWindow::weekViewChanged);
    connect(branchCB, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          this, &MainWindow::branchChanged);

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
    //Everything it's ok. So we can change startDate and update
    // the weekViewToolBar.
    m_startDate = spreadsheet->startDate();
    updateWeekCBItems();
    ui->actionBranches_Full_Name->setEnabled(true);
    ui->actionBranches_Full_Name->setChecked(false);
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

void MainWindow::updateWeekCBItems()
{
    weekCB->clear();
    QString lst;
    QDate cdate = m_startDate;
    QDate ddate = m_startDate.addDays(6);
    QString format = "dd/MM/yyyy";
    for(auto i = 1; i <= 7; ++i) {
        lst = tr("Week %1 ( %2 - %3 )").arg(i)
               .arg(cdate.toString(format))
               .arg(ddate.toString(format));
        weekCB->addItem(lst,cdate);
        cdate = ddate.addDays(1);
        ddate = cdate.addDays(6);
    }
    weekCB->setCurrentIndex(0);
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
        updateWeekCBItems();
        setCurrentFile("");
        ui->actionBranches_Full_Name->setEnabled(true);
        ui->actionBranches_Full_Name->setChecked(false);
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
    AboutDialog aboutDlg(this);
    aboutDlg.exec();
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
    const int ItemSize = 256;
#if !defined(QT_NO_PRINTER) && !defined(QT_NO_PRINTDIALOG)
      QPrinter printer(QPrinter::ScreenResolution);
      QPrintDialog dlg(&printer);
      PrintView view;
      QAbstractItemModel *model;
      switch(m_centralView->cvStackedWnd()->currentIndex()) {
      case 0: //QEmployeeShiftsTable
          model = m_centralView->employeeShiftsTable()->model();
          break;
      case 1:
          model = m_centralView->employeeReportTable()->tableModel();
          break;
      case 2:
          model = m_centralView->staffReportTable()->model();
          printer.setOrientation(QPrinter::Landscape);
          break;
      }

      view.setModel(model);
      view.resizeColumnsToContents();
      view.resizeRowsToContents();

      //connect(&dlg, &QPrintPreviewDialog::paintRequested, &view, &PrintView::print);
      if (dlg.exec() != QDialog::Accepted) {
          return;
      }

//      QPainter painter;
//      painter.begin(&printer);

      int rows = model->rowCount(QModelIndex());
      int columns = model->columnCount(QModelIndex());
      int sourceWidth = (columns + 1) * ItemSize;
      int sourceHeight = (rows + 1) * ItemSize;

     // painter.save();

      double xscale = printer.pageRect().width() / double(sourceWidth);
      double yscale = printer.pageRect().height() / double(sourceHeight);
      double scale = qMin(xscale, yscale);

//      painter.translate(printer.paperRect().x() + printer.pageRect().width() / 2,
//                        printer.paperRect().y() + printer.pageRect().height() / 2);
//      painter.scale(scale,scale);
//      painter.translate(-sourceWidth / 2, -sourceHeight / 2);

      QStyleOptionViewItem option;
      QModelIndex parent = QModelIndex();

      QProgressDialog progress(tr("Printing..."), tr("Cancel"), 0, rows, this);
      progress.setWindowModality(Qt::ApplicationModal);
      float y = ItemSize / 2;

      for (int row = 0; row < rows; ++row) {
          progress.setValue(row);
          qApp->processEvents();
          if (progress.wasCanceled())
              break;

          float x = ItemSize / 2;

//          for (int column = 0; column < columns; ++column) {
//              option.rect = QRect(int(x), int(y), ItemSize, ItemSize);
//              view.itemDelegate()->paint(&painter, option,
//                                          model->index(row, column, parent));
//              x = x + ItemSize;
//          }
//          y = y + ItemSize;
      }
      progress.setValue(rows);

//      painter.restore();
//      painter.end();

      if (progress.wasCanceled()) {
          QMessageBox::information(this, tr("Printing canceled"),
                                   tr("The printing process was canceled."), QMessageBox::Cancel);
      }
      view.print(&printer);
  #else
      QMessageBox::information(this, tr("Printing canceled"),
                               tr("Printing is not supported on this Qt build"), QMessageBox::Cancel);
  #endif
    //m_centralView->employeeReportTable()->print();

}

void MainWindow::filePrintPreview()
{

}

void MainWindow::printPreview(QPrinter *printer)
{

}

void MainWindow::showPreferencesDialog()
{
    if (!settingsDlg)
        settingsDlg = new AlShiftsSettingsDialog(this);

    if(settingsDlg->exec() == QDialog::Accepted) {
         qDebug()<< "Options Dialog return OK";
         m_centralView->employeeShiftsTable()->updateVHeader();
    }

}

void MainWindow::updateStatusBar()
{
    locationLabel->setText(m_centralView->employeeShiftsTable()->currentLocation());
}

void MainWindow::changeView(bool checked)
{
    m_centralView->cvStackedWnd()->setCurrentIndex((int)checked);
    weekViewToolBar->setVisible(checked);
    if(!branchCB->isEnabled())
        branchCB->setEnabled(true);
    m_centralView->employeeReportTable()->createWeekReport(weekCB->currentData().toDate());
}

void MainWindow::weekViewChanged(int week)
{
    int viewIndex = m_centralView->cvStackedWnd()->currentIndex();

    qDebug() << "Week view changed! Index : " << week << endl;
    QString fn = tr("Week_%1-").arg(week+1);
    QDate d = weekCB->currentData().toDate();
    fn += d.toString("dd_MM-");
    d= d.addDays(6);
    fn += d.toString("dd_MM");
    if(viewIndex == 1) {
        if(week >= 0) {
            m_centralView->employeeReportTable()->setCVSFilename(fn);
            m_centralView->employeeReportTable()->createWeekReport(weekCB->currentData().toDate());
        }
    } else if (viewIndex == 2) {
        ///ToDO : Handle your code here for stuff view.
        m_centralView->staffReportTable()->setCSVFilename(fn);
        m_centralView->staffReportTable()->createWeekReport(weekCB->currentData().toDate());
    }
}

void MainWindow::branchChanged(int branch)
{
    /**
      Write your code here!!
      Activated when the user change the branch.
      */
    qDebug() << "Branch view changed! Index : " << branch << endl;
    if( m_centralView->cvStackedWnd()->currentIndex() == 1) {
        QString branchID = branchCB->currentData().toString();
        m_centralView->employeeReportTable()->branchChanged(branchID);
    }
}

void MainWindow::toggleStaffWeekView(bool checked)
{
    int index = checked ? 2 : 0;
    m_centralView->cvStackedWnd()->setCurrentIndex(index);
    if(index == 2 && !weekViewToolBar->isVisible()) {
        weekViewToolBar->setVisible(true);
        m_centralView->staffReportTable()->createWeekReport(weekCB->currentData().toDate());
        branchCB->setEnabled(false);
    }

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
