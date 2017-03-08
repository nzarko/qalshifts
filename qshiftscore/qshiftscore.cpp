#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>
#include <QApplication>

#include "qshiftscore.h"

namespace Algorithmos {
QShiftsCore::QShiftsCore()
{
    init();
}

QShiftsCore::~QShiftsCore()
{
    delete m_solver;
}

void QShiftsCore::init()
{
    m_solver = Q_NULLPTR;

    QDir path ;
    path = QDir(path.currentPath());
    qDebug() << "Current path : "<< path.absolutePath() << endl;
    path.cdUp();
    if(path.cd("Data")) {
        qDebug() << "Current path : " << path.absolutePath() << endl;
        QFile file(path.absolutePath()+"/employeeswb.json");
        QFileInfo info(file);
        qDebug() << "File path :" << info.absoluteFilePath() << endl;
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            qCritical() << "Error reading employees file." << endl;
            return;
        }
        QFile file1(path.absolutePath()+"/fe_matrix.txt");
        QFileInfo info1(file1);
        qDebug() << "File path :" << info1.absoluteFilePath() << endl;
        if(!file1.open(QIODevice::ReadOnly | QIODevice::Text)){
            qCritical() << "Error reading matrix file." << endl;
        }

        QJsonParseError jerror;
        QJsonDocument jdoc= QJsonDocument::fromJson(file.readAll(),&jerror);
        if(jerror.error != QJsonParseError::NoError) {
            qCritical() << "Error while reading employees file : \n"
                        <<jerror.errorString() << endl;
            return;
        }

        QJsonObject obj = jdoc.object();
        qDebug() << obj.size() << endl;
        if (obj["Employees"].isArray())
        {
            QString str_id, name,pos;
            QEmployee *employee = Q_NULLPTR;
            EmployeeType eType;

            QJsonArray jarr = obj["Employees"].toArray();
            for(auto v : jarr) {
                QJsonObject val = v.toObject();
                str_id = val["id"].toString();
                name = val["name"].toString();
                pos = val["position"].toString();
                eType = parseEmployeeType(pos);
                qDebug() << "ID : " << str_id << "\n"
                         << "Name : " << name  << "\n"
                         << "Position : " << pos;
                QJsonArray branches = val["branches"].toArray();
                QStringList brList;
                for(auto b : branches) {
                    brList << b.toString();
                }
                employee = createEmployee(str_id.toInt(),name,eType,brList);
                switch (eType) {
                case Algorithmos::BMANAGER:
                    bManagers.push_back(employee);
                    break;
                case Algorithmos::BFUELMANAGER:
                    bfManagers.push_back(employee);
                    break;
                case Algorithmos::FUELMANAGER:
                    bEmployees.push_back(employee);
                    break;
                default:
                    qDebug() << "Error " << __FILE__ << " " << __LINE__ << endl;
                    break;
                }
                qDebug() << "Branches : " << brList << endl;
            }
            qDebug() << "Employees Loaded by category : " << "\n";
            qDebug() << "Managers :\n" << bManagers << endl;
            qDebug() << "Fuel Managers :\n" << bfManagers << endl;
            qDebug() << "Fuel Employees :\n" << bEmployees << endl;
            //Now it's time to initialize our solver
            m_solver = new QShiftSolver(bManagers,bfManagers, bEmployees);
            m_solver->setEmployeeMatrixFile(info1.absoluteFilePath());
        }
    } else {
        qDebug() << "Error reading file. Current path : " << path.absolutePath() << endl;
    }
}

QShiftSolver *QShiftsCore::solver()
{
    return m_solver;
}

QVector<QEmployee *> QShiftsCore::branchManagers()
{
    return bManagers;
}

QVector<QEmployee *> QShiftsCore::branchFuelManagers()
{
    return bfManagers;
}

QVector<QEmployee *> QShiftsCore::fuelEmployees()
{
    return bEmployees;
}
}
