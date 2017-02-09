#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "centralview.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    move(500,100);
    m_centralView = new CentralView();
    setCentralWidget(m_centralView);

}

MainWindow::~MainWindow()
{
    delete m_centralView;
    delete ui;
}
