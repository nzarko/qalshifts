#include <QString>

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>

#include "employeeeditorform.h"
#include "ui_employeeeditorform.h"

EmployeeEditorForm::EmployeeEditorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EmployeeEditorForm)
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

    ui->employeesTV->clear();
    initTV();

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
}

void EmployeeEditorForm::on_addBranchToList_clicked()
{
    QListWidgetItem *lItem = new QListWidgetItem(ui->branchesCBox->currentText());
    lItem->setData(Qt::UserRole,ui->branchesCBox->currentData());
    ui->availableBranchesLV->insertItem(0,lItem);
    lItem->setToolTip(lItem->data(Qt::UserRole).toString());
}

void EmployeeEditorForm::initTV()
{
    QTreeWidget *tv = ui->employeesTV;
    rootManItem = new QTreeWidgetItem(tv);
    rootManItem->setText(0,tr("Branch Managers"));
    rootManItem->setData(0,Qt::UserRole,Algorithmos::BMANAGER);

    rootFuelManItem = new QTreeWidgetItem(tv);
    rootFuelManItem->setText(0,tr("Branch - Fuel Managers"));
    rootFuelManItem->setData(0,Qt::UserRole,Algorithmos::BFUELMANAGER);

    rootFuelEmplItem = new QTreeWidgetItem(tv);
    rootFuelEmplItem->setText(0,tr("Fuel Employees"));
    rootFuelEmplItem->setData(0,Qt::UserRole, Algorithmos::FUELMANAGER);
}

void EmployeeEditorForm::on_employeesTV_itemClicked(QTreeWidgetItem *item, int column)
{
    using namespace Algorithmos;
    if(item->parent()) { //item is child
        QEmployee *employee = employees[item->data(column,Qt::UserRole).toInt()];
        if(employee) {
            ui->nameLE->setText(employee->name());
            ui->employeeTypeLE->setText(EmployeeTypeName(employee->employeeType()));
            ui->availableBranchesLV->clear();
            for(auto br : employee->branches()) {
                QListWidgetItem *lItem = new QListWidgetItem(branchFullName.value(br),ui->availableBranchesLV);
                lItem->setData(Qt::UserRole,branchFullName.key(lItem->text()));
                lItem->setToolTip(lItem->data(Qt::UserRole).toString());
            }
        }
    } else
        qDebug() << "Top level item clicked!" << endl;
}
