#include <QString>

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>
#include <QStringList>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>

#include "employeeeditorform.h"
#include "ui_employeeeditorform.h"

EmployeeEditorForm::EmployeeEditorForm(QAbstractSettingsWidget *parent) :
    QAbstractSettingsWidget(parent),
    ui(new Ui::EmployeeEditorForm),
    isEmployeeTVPopulated(false),
    curEmployee(nullptr)
{
    ui->setupUi(this);

    branchFullName.insert("BR2", tr("Kalampakas 104"));
    branchFullName.insert("BR3",tr("Pylis 39"));
    branchFullName.insert("BR4",tr("Pylis 98"));
    branchFullName.insert("BR5", tr("Karditsis 54"));
    ui->branchesCBox->addItem(branchFullName.value("BR2"),branchFullName.key(branchFullName.value("BR2")));
    ui->branchesCBox->addItem(branchFullName.value("BR3"),branchFullName.key(branchFullName.value("BR3")));
    ui->branchesCBox->addItem(branchFullName.value("BR4"),branchFullName.key(branchFullName.value("BR4")));
    ui->branchesCBox->addItem(branchFullName.value("BR5"),branchFullName.key(branchFullName.value("BR5")));

    ui->emTypeCB->addItem(Algorithmos::EmployeeTypeName(Algorithmos::BMANAGER),Algorithmos::BMANAGER);
    ui->emTypeCB->addItem(Algorithmos::EmployeeTypeName(Algorithmos::BFUELMANAGER),Algorithmos::BFUELMANAGER);
    ui->emTypeCB->addItem(Algorithmos::EmployeeTypeName(Algorithmos::FUELMANAGER), Algorithmos::FUELMANAGER);
    ui->emTypeCB->addItem(Algorithmos::EmployeeTypeName(Algorithmos::INVALID), Algorithmos::INVALID);

    ui->employeesTV->clear();
    initTV();
    ui->employeesTV->addAction(ui->actionNew_Employee);
    ui->employeesTV->addAction(ui->actionEdit);
    ui->employeesTV->addAction(ui->actionDelete);
    ui->employeesTV->setContextMenuPolicy(Qt::ActionsContextMenu);

    QDir path ;
    path = QDir(path.currentPath());

    path.cdUp();
    if(path.cd("Data")) {
        QString filename = path.absolutePath()+"/employeeswb.json";
        populateTVFromFile(filename);
    }
}

EmployeeEditorForm::~EmployeeEditorForm()
{
    qDeleteAll(employees);
    employees.clear();
    delete ui;
}

void EmployeeEditorForm::populateTVFromFile(const QString &fileName)
{
    using namespace Algorithmos;

    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qCritical() <<"Settings Dialog : Error reading employees file." << endl;
        return;
    }

    QJsonParseError jerror;
    QJsonDocument jdoc= QJsonDocument::fromJson(file.readAll(),&jerror);
    if(jerror.error != QJsonParseError::NoError) {
        qCritical() << "Error while reading employees file : \n"
                    <<jerror.errorString() << endl;
        return;
    }

    employees.clear();

    QJsonObject obj = jdoc.object();
    qDebug() << obj.size() << endl;
    if (obj["Employees"].isArray())
    {
        QString str_id, name,pos;
        QEmployee *employee = Q_NULLPTR;
        EmployeeType eType;

        QJsonArray jarr = obj["Employees"].toArray();
        int index = 0;
        for(auto v : jarr) {
            QJsonObject val = v.toObject();
            str_id = val["id"].toString();
            name = val["name"].toString();
            pos = val["position"].toString();
            eType = parseEmployeeType(pos);

            QJsonArray branches = val["branches"].toArray();
            QStringList brList;
            for(auto b : branches) {
                brList << b.toString();
            }
            employee = createEmployee(str_id.toInt(),name,eType,brList);
            QTreeWidgetItem *childItem = new QTreeWidgetItem();
            childItem->setText(0,employee->name());
            switch (eType) {
            case Algorithmos::BMANAGER:
                rootManItem->addChild(childItem);
                break;
            case Algorithmos::BFUELMANAGER:
                rootFuelManItem->addChild(childItem);
                break;
            case Algorithmos::FUELMANAGER:
                rootFuelEmplItem->addChild(childItem);
                break;
            default:
                qDebug() << "Error " << __FILE__ << " " << __LINE__ << endl;
                break;
            }
            employees.push_back(employee);
            childItem->setData(0,Qt::UserRole,index);
            ++index;
        }
    }
    isEmployeeTVPopulated = true;
}

bool EmployeeEditorForm::saveEmployees() const
{
    QFile saveFile(Algorithmos::getFilePath(Algorithmos::EMPLOYEES));

    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return false;
    }

    QJsonObject root;
    QJsonArray rootArray;
    for(int i = 0; i < employees.size(); ++i) {
        QJsonObject obj;
        employees[i]->write(obj);
        rootArray.append(obj);
    }
    root["Employees"] = rootArray;

    QJsonDocument saveDoc(root);
    saveFile.write(saveDoc.toJson());

    return true;
}

bool EmployeeEditorForm::applyChanges() const
{
    return saveEmployees();
}

void EmployeeEditorForm::on_addBranchToList_clicked()
{
    //Update employees
    QTreeWidgetItem *cItem = ui->employeesTV->currentItem();
    Algorithmos::QEmployee *ce = employees[cItem->data(0,Qt::UserRole).toInt()];
    int l1 = ce->branches().size();
    qDebug() << "Current employee before : " << ce << endl;
    ce->addBranch(ui->branchesCBox->currentData().toString());
    qDebug() << "Current employee after : "  << ce << endl;
    int l2 = ce->branches().size();
    if(l1 != l2) {
        QListWidgetItem *lItem = new QListWidgetItem(ui->branchesCBox->currentText());
        lItem->setData(Qt::UserRole,ui->branchesCBox->currentData());
        ui->availableBranchesLV->insertItem(0,lItem);
        lItem->setToolTip(lItem->data(Qt::UserRole).toString());
    }
}

void EmployeeEditorForm::initTV()
{
    QTreeWidget *tv = ui->employeesTV;
    rootManItem = new QTreeWidgetItem(tv);
    rootManItem->setText(0,tr("Branch Managers"));
    rootManItem->setData(0,Qt::UserRole,Algorithmos::BMANAGER);
    rootManItem->setExpanded(true);

    rootFuelManItem = new QTreeWidgetItem(tv);
    rootFuelManItem->setText(0,tr("Branch - Fuel Managers"));
    rootFuelManItem->setData(0,Qt::UserRole,Algorithmos::BFUELMANAGER);
    rootFuelManItem->setExpanded(true);

    rootFuelEmplItem = new QTreeWidgetItem(tv);
    rootFuelEmplItem->setText(0,tr("Fuel Employees"));
    rootFuelEmplItem->setData(0,Qt::UserRole, Algorithmos::FUELMANAGER);
    rootFuelEmplItem->setExpanded(true);
}

void EmployeeEditorForm::clearForm()
{
    ui->nameLE->clear();
    ui->branchesCBox->setCurrentIndex(0);
    ui->availableBranchesLV->clear();
}

void EmployeeEditorForm::on_employeesTV_itemClicked(QTreeWidgetItem *item, int column)
{
    using namespace Algorithmos;
    if(item->parent()) { //item is child
        curEmployee = employees[item->data(column,Qt::UserRole).toInt()];
        if(curEmployee) {
            ui->nameLE->setText(curEmployee->name());
            ui->emTypeCB->setCurrentIndex((int)curEmployee->employeeType());
            ui->availableBranchesLV->clear();
            for(auto br : curEmployee->branches()) {
                QListWidgetItem *lItem = new QListWidgetItem(branchFullName.value(br),ui->availableBranchesLV);
                lItem->setData(Qt::UserRole,branchFullName.key(lItem->text()));
                lItem->setToolTip(lItem->data(Qt::UserRole).toString());
            }
        }
    } else
        qDebug() << "Top level item clicked!" << endl;
}

void EmployeeEditorForm::on_employeesTV_itemParentChanged(QTreeWidgetItem *droppedItem)
{
    qDebug() << "on_employeesTV_itemParentChanged ";
    using namespace Algorithmos;
    QTreeWidgetItem *parent = droppedItem->parent();
    if(parent) {
        EmployeeType eType = (EmployeeType)parent->data(0,Qt::UserRole).toInt();
        QEmployee *empl = employees[droppedItem->data(0,Qt::UserRole).toInt()];
        qDebug() << "Employee Before " << empl ;
        if(empl) {
            empl->setEmployeeType(eType);
            qDebug() << "Employee After " << empl;
        }
    }
}

void EmployeeEditorForm::on_emTypeCB_currentIndexChanged(int index)
{
    qDebug() << "on_emTypeCB_currentIndexChanged ";
    qDebug() << "emTypeCB index changed : " << index << endl;
    if(!isEmployeeTVPopulated)
        return;

    //ui->employeesTV->setUpdatesEnabled(false);
    if (index != 3) {
        QTreeWidgetItem *pItem = ui->employeesTV->currentItem();
        QTreeWidgetItem *parent = pItem->parent();
        qDebug() << "Parent of selected item : " << parent->text(0) << endl;
        //qDebug() << "index of parent : " << ui->employeesTV->indexOfTopLevelItem(parent) << endl;
        if(pItem && parent) {
            int pIndex = ui->employeesTV->indexOfTopLevelItem(parent);
            if(pIndex == index) return;
            QTreeWidgetItem *p2Item = parent->takeChild(parent->indexOfChild(pItem));
            ui->employeesTV->topLevelItems().at(index)->addChild(p2Item); //Add it to proper topLevelItem.
            Algorithmos::QEmployee *empl = employees[p2Item->data(0,Qt::UserRole).toInt()];
            qDebug() << "Employee Before " << empl ;
            if(empl) {
                empl->setEmployeeType((Algorithmos::EmployeeType)index);
                qDebug() << "Employee After " << empl;
            }
            ui->employeesTV->setCurrentItem(p2Item);
        }
    }
    //ui->employeesTV->setUpdatesEnabled(true);
}

void EmployeeEditorForm::on_updateBtn_clicked()
{

}

void EmployeeEditorForm::on_nameLE_textEdited(const QString &arg1)
{
    using namespace Algorithmos;
    auto cItem = ui->employeesTV->currentItem();
    QEmployee *curEmployee = employees[cItem->data(0,Qt::UserRole).toInt()];
    cItem->setText(0,arg1);
    curEmployee->setName(arg1);

    qDebug() << "on_nameLE_textEdited --> employees vector : "
             << employees[cItem->data(0,Qt::UserRole).toInt()]->name() << endl;
}

void EmployeeEditorForm::on_actionNew_Employee_triggered()
{
    clearForm();
    ui->employeesTV->addEmptyItem();
    QTreeWidgetItem *parent = ui->employeesTV->currentItem()->parent();
    if(parent) {
        int index = ui->employeesTV->indexOfTopLevelItem(parent);
        ui->nameLE->setText(ui->employeesTV->currentItem()->text(0));
        ui->nameLE->setFocus();
        Algorithmos::QEmployee *ne = Algorithmos::createEmployee(employees.size(),tr("New Employee"),
                                                                 (Algorithmos::EmployeeType)index,
                                                                 QStringList());
        employees.push_back(ne);
        ui->employeesTV->currentItem()->setData(0,Qt::UserRole,employees.size()-1);
        qDebug() << "Currently added employee : " << employees.last();
    }
}

void EmployeeEditorForm::on_actionEdit_triggered()
{
    ui->nameLE->selectAll();
    ui->nameLE->setFocus();
}

void EmployeeEditorForm::on_employeesTV_itemChanged(QTreeWidgetItem *item, int column)
{
    qDebug() << "Item : " << item->text(column) << " changed" << endl;
}

void EmployeeEditorForm::on_actionDelete_triggered()
{
    qDebug() << "on_actionDelete_triggered :" << ui->employeesTV->currentItem()->text(0);
    QTreeWidgetItem *cItem = ui->employeesTV->currentItem();
    QTreeWidgetItem *parent = cItem->parent();

    if(parent) {
        int ret = QMessageBox::question(this, tr("Are you sure ?"),
                                        tr("Are you sure you want to delete %1\nfrom your employees list?")
                                        .arg(cItem->text(0)),
                                        QMessageBox::Yes | QMessageBox::No);
        if(ret == QMessageBox::Yes) {
            qDebug() << "employees vector before : " << employees;
            employees.remove(cItem->data(0,Qt::UserRole).toInt());
            parent->takeChild(parent->indexOfChild(cItem));
            qDebug() << "employees vector after : " << employees;
        }
    }
}

void EmployeeEditorForm::on_deleteFromListBtn_clicked()
{
    qDebug() << "on_deleteFromListBtn_clicked : " ;
    QListWidgetItem *cListItem = ui->availableBranchesLV->currentItem();
    QTreeWidgetItem *cItem = ui->employeesTV->currentItem();
    Algorithmos::QEmployee *ce = employees[cItem->data(0,Qt::UserRole).toInt()];
    qDebug() << "Employee before : " << ce;
    ce->takeBranch(cListItem->data(Qt::UserRole).toString());
    qDebug() << "Employee after : " << ce;

    ui->availableBranchesLV->takeItem(ui->availableBranchesLV->currentIndex().row());
}
