#ifndef EMPLOYEEEDITORFORM_H
#define EMPLOYEEEDITORFORM_H

#include <QWidget>
#include <QVector>
#include <QMap>

#include "qemployee.h"

QT_BEGIN_NAMESPACE
class QTreeWidgetItem;
QT_END_NAMESPACE

namespace Ui {
class EmployeeEditorForm;
}

class EmployeeEditorForm : public QWidget
{
    Q_OBJECT

public:
    explicit EmployeeEditorForm(QWidget *parent = 0);
    ~EmployeeEditorForm();
    void populateTVFromFile(const QString &fileName);


private slots:
    void on_addBranchToList_clicked();

    void on_employeesTV_itemClicked(QTreeWidgetItem *item, int column);

private:
    Ui::EmployeeEditorForm *ui;
    QTreeWidgetItem *rootManItem;
    QTreeWidgetItem *rootFuelManItem;
    QTreeWidgetItem *rootFuelEmplItem;
    QVector<Algorithmos::QEmployee*> employees;
    QMap<QString, QString> branchFullName;

    void initTV();
};

#endif // EMPLOYEEEDITORFORM_H
