#include <QLayout>
#include <QApplication>

#include "centralview.h"
#include "qemployeeshiftstable.h"
#include "legendform.h"
#include "qemployeeshiftsweeklyreport.h"

CentralView::CentralView(QWidget *parent) : QWidget(parent)
{
    m_stackWnd = new QStackedWidget(parent);
    m_emplTable = new QEmployeeShiftsTable();
    m_legendForm = new LegendForm();
    QVBoxLayout *vsBox = new QVBoxLayout();
    vsBox->addWidget(m_legendForm);
    vsBox->addWidget(m_emplTable);
    QWidget *wnd = new QWidget();
    wnd->setLayout(vsBox);
    m_stackWnd->addWidget(wnd);
    QVBoxLayout *vBox = new QVBoxLayout();

    vBox->addWidget(m_stackWnd);
    setLayout(vBox);

    m_reportWnd = new QEmployeeShiftsWeeklyReport();
    m_stackWnd->addWidget(m_reportWnd);
    m_stackWnd->setCurrentIndex(0);
}

void CentralView::populate()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_emplTable->populate();
    QApplication::restoreOverrideCursor();
}
