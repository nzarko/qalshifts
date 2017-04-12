#ifndef EMPLOYEEEDITORFORM_H
#define EMPLOYEEEDITORFORM_H

#include <QVector>
#include <QMap>

#include "qemployee.h"
#include "qabstractsettingswidget.h"

QT_BEGIN_NAMESPACE
class QTreeWidgetItem;
QT_END_NAMESPACE

namespace Ui {
class EmployeeEditorForm;
}

class EmployeeEditorForm : public QAbstractSettingsWidget
{
    Q_OBJECT

public:
    explicit EmployeeEditorForm(QAbstractSettingsWidget *parent = 0);
    ~EmployeeEditorForm();
    void populateTVFromFile(const QString &fileName);

public Q_SLOTS:
    /**
     * @brief saveEmployees Saves employees updates in file.
     * @return true if everything went OK, false in other case.
     */
    bool saveEmployees() const;
    bool applyChanges() const;
private slots:
    /**
     * @brief on_addBranchToList_clicked
     * Adds branches to selected employee
     */
    void on_addBranchToList_clicked();

    /**
     * @brief on_employeesTV_itemClicked
     * @param item The clicked item
     * @param column The item column
     * Occurs when the user clicks an item on Employee tree view
     * Fills the controls with employee info
     */
    void on_employeesTV_itemClicked(QTreeWidgetItem *item, int column);

    /**
     * @brief on_employeesTV_itemParentChanged
     * @param droppedItem The item to be dropped.
     * Covers the drag and drop operation. Items can be dropped only on
     * topLevelItems. When this happed this function changes the employee
     * type by calling the Algorithmos::QEmployee::setEmployeeType
     */
    void on_employeesTV_itemParentChanged(QTreeWidgetItem *droppedItem);

    void on_emTypeCB_currentIndexChanged(int index);

private:
    Ui::EmployeeEditorForm *ui;
    QTreeWidgetItem *rootManItem;
    QTreeWidgetItem *rootFuelManItem;
    QTreeWidgetItem *rootFuelEmplItem;
    QVector<Algorithmos::QEmployee*> employees;
    QMap<QString, QString> branchFullName;

    void initTV();
    bool isEmployeeTVPopulated;

    Algorithmos::QEmployee *curEmployee; //The employee selected in ui->employeeTV
};

#endif // EMPLOYEEEDITORFORM_H
