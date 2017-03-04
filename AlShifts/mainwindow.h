#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>

namespace Ui {
class MainWindow;
}

class CentralView;
class QDateSelector;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    CentralView *m_centralView;

    void setupActions();
    QDate m_startDate;
    QDateSelector* dateSelectorDlg;


public slots:
    void selectStartDate();

};

#endif // MAINWINDOW_H
