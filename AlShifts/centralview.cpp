#include <QLayout>

#include "centralview.h"
#include "qemployeeshiftstable.h"

CentralView::CentralView(QWidget *parent) : QWidget(parent)
{
    m_stackWnd = new QStackedWidget(parent);
    m_emplTable = new QEmployeeShiftsTable();
    m_stackWnd->addWidget(m_emplTable);
    QVBoxLayout *vBox = new QVBoxLayout();
    vBox->addWidget(m_stackWnd);
    setLayout(vBox);
}
