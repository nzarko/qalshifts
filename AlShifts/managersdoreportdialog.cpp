#include "managersdoreportdialog.h"
#include "ui_managersdoreportdialog.h"
#include "qemployeeshiftstable.h"

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QTextDocument>
#include <QWebEngineView>
#include <QPrinter>
#include <QPrintDialog>
#include <QPushButton>
#include <QTextCodec>
#include <QByteArray>
#include <QDebug>

ManagersDOReportDialog::ManagersDOReportDialog(QEmployeeShiftsTable *shiftsTbl, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ManagersDOReportDialog),
    m_shiftsView(shiftsTbl)
{
    ui->setupUi(this);
    QPushButton *printBtn = ui->buttonBox->addButton(tr("Print"),QDialogButtonBox::ActionRole);
    ui->buttonBox->addButton(QDialogButtonBox::Close);

    createReport();

    QString htmlFile = "file:///" + QDir::tempPath() + "/qalshifts_managers_report.html";
    htmlFileURL = QUrl(htmlFile);
    qDebug() << "ManagersDOReportDialog Ctor --> htmlFile path : " << htmlFile << endl;

    QString str = "Γεια σου κόσμε!";
    qDebug() << "ManagersDOReportDialog Ctor --> "<< str << endl;
    ui->webView->load(QUrl(htmlFile));

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
#if defined (QWEBENGINEPAGE_PRINT)
    connect(printBtn, &QPushButton::clicked, this, &ManagersDOReportDialog::print);
#endif
}

ManagersDOReportDialog::~ManagersDOReportDialog()
{
    delete ui;
}

void ManagersDOReportDialog::createReport()
{
    //Main functionality!
    //First create the Dayoff table
    QString tableHtml = QString("\n<table style=\"width:100%\">\n"
                                "<tbody>\n<tr>\n<th>%1</th>\n"
                                "<th style=\"text-align:center;\" colspan=\"8\">"
                                "%2</th>\n"
                                "</tr>\n")
            .arg(tr("Name"))
            .arg(tr("Date"));
    //Table main body
    if(m_shiftsView) {
        for(int i = 0; i < 7; ++i) {
            tableHtml += "<tr><td>"+m_shiftsView->verticalHeaderItem(i)->text() + "</td>\n";
            for(int j = 0; j < m_shiftsView->columnCount(); ++j) {
                if(m_shiftsView->item(i,j)->data(Algorithmos::STIROLE).toInt() == 3) {
                    tableHtml += "<td>" + m_shiftsView->horizontalHeaderItem(j)->text() + "</td>";
                }
            }
            tableHtml += "</tr>";
        }
        tableHtml += "</tbody>\n</table>";
    }

    QFile reportFile(Algorithmos::getFilePath(Algorithmos::DOREPORT));
    if(!reportFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "ManagersDOReportDialog::createReport --> "
                 << "Could not open " << reportFile.fileName() << endl;
        return;
    }
    reportStr = reportFile.readAll();
    int startPos = reportStr.indexOf("<h2>",Qt::CaseInsensitive);
    if(startPos != -1) {
        reportStr.insert(startPos+4,tr("Managers Dayoff report"));
        startPos = reportStr.indexOf("</h2>");
        if(startPos != -1)
            reportStr.insert(startPos+5,tableHtml);
    }

    qDebug() << "ManagersDOReportDialog::createReport \n" << "Table body source : \n"
             << reportStr << endl;

    QFile saveReportFile(QDir::tempPath() + "/qalshifts_managers_report.html");
    if(!saveReportFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "ManagersDOReportDialog::createReport --> "
                 << "Could not open " << saveReportFile.fileName()
                 <<"for writing." << endl;
        return;
    }
    QTextStream ts(&saveReportFile);
    ts.setCodec("UTF-8");
    ts << reportStr;

    saveReportFile.close();
}

#if defined (QWEBENGINEPAGE_PRINT)
void ManagersDOReportDialog::print()
{
//    qDebug() << "ManagersDOReportDialog::print triggered" << endl;
//    QFile file(QDir::tempPath() + "/qalshifts_managers_report.html");
//    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
//        return;

//    QString htmlContent;
//    QTextStream in(&file);
//    in.setCodec("UTF-8");
//    htmlContent = in.readAll();

//    QTextDocument *document = new QTextDocument();
//    document->setHtml(htmlContent);

//    QPrinter printer;

//    QPrintDialog *dialog = new QPrintDialog(&printer, this);
//    if (dialog->exec() != QDialog::Accepted)
//        return;

//    document->print(&printer);

//    delete document;
#ifndef QT_NO_PRINTDIALOG
    QPrinter printer;
    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle(tr("Print Dayoff Report"));
    if(dialog->exec() != QDialog::Accepted)
        return;

    ui->webView->page()->print(&printer);
#endif
}
#endif
