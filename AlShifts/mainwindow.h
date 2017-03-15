#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>

namespace Ui {
class MainWindow;
}

QT_BEGIN_NAMESPACE
class QMenu;
class QAction;
class QPrinter;
class QLabel;
class QCloseEvent;
QT_END_NAMESPACE

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

    QLabel *locationLabel;
    QString curFile;
    QStringList recentFiles;
    QAction *separatorAction;

    void setupActions();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    bool loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);

    QString strippedName(const QString &fullFileName);
    void updateRecentFileActions();
    void updateRecentFiles(const QString &fileName);

    enum { MaxRecentFiles = 20 };
    QAction *recentFileActions[MaxRecentFiles];


    QDate m_startDate;
    QDateSelector* dateSelectorDlg;
    QMenu *recentFilesMenu;


public slots:
    void selectStartDate();
private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    bool saveAll();
    void about();
    void documentWasModified();
    void openRecentFile();
    void setCurrentFile(const QString &fileName);
    void fileprint();
    void filePrintPreview();
    void printPreview(QPrinter *printer);
    void showPreferencesDialog();
    void updateStatusBar();

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
