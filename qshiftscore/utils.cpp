#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <algorithm>
#include <functional>
#include <numeric>
#include <set>
#include <stdio.h>

#include <QObject>
#include <QTextCodec>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDebug>

#include "utils.h"

namespace Algorithmos {
    QString EmployeeTypeName(EmployeeType type)
    {
        QString name;
        switch (type)
        {
        case Algorithmos::BMANAGER:
            name = QObject::tr("Branch Manager");
            break;
        case Algorithmos::BFUELMANAGER:
            name = QObject::tr("Branch Fuel Manager");
            break;
        case Algorithmos::FUELMANAGER:
            name = QObject::tr("Fuel Employee");
            break;
        case Algorithmos::INVALID:
            name = QObject::tr("Invalid");
            break;
        }
        return name;
    }

    QString EmployeeTypeShortName(EmployeeType type)
    {
        QString name;
        switch (type)
        {
        case Algorithmos::BMANAGER:
            name = QObject::tr("YK");
            break;
        case Algorithmos::BFUELMANAGER:
            name = QObject::tr("YB");
            break;
        case Algorithmos::FUELMANAGER:
            name = QObject::tr("B");
            break;
        case Algorithmos::INVALID:
            name = QObject::tr("Invalid");
            break;
        }
        return name;
    }

    EmployeeType setEmployeeTypeByName(const QString & name)
    {
        EmployeeType type;
        if (name == "YK")
            type = Algorithmos::BMANAGER;
        else if (name == "YB")
            type = Algorithmos::BFUELMANAGER;
        else if (name == "B")
            type = Algorithmos::FUELMANAGER;
        else if (name == "YB-YK")
            type = Algorithmos::BFUELMANAGER;
        else {
            type = Algorithmos::INVALID;
            qDebug() << "Could not parse employee type. Check for typo in you employees file." << endl;
        }

        return type;
    }

    QString to_qstring(const std::string & s)
    {
        QByteArray arr = s.c_str();
        QTextCodec *codec = QTextCodec::codecForName("Windows-1253");
        return codec->toUnicode(arr);
    }

    EmployeeType parseEmployeeType(const QString &str_type)
    {
        EmployeeType type;
        if (str_type == "YK")
            type = Algorithmos::BMANAGER;
        else if (str_type == "YB")
            type = Algorithmos::BFUELMANAGER;
        else if (str_type == "B")
            type = Algorithmos::FUELMANAGER;
        else if (str_type == "YB-YK")
            type = Algorithmos::BFUELMANAGER;
        else{
            type = Algorithmos::INVALID;
            qDebug() << "Could not parse employee type. Check for typo in you employees file." << endl;
            qDebug() << "From employees file I get : " << str_type << " as type. Is this correct??" << endl;
        }

        return type;
    }

    QString shiftName(ShiftType stype)
    {
        QString name;
        switch(stype) {
        case EARLY:
            name = QObject::tr("Early");
            break;
        case LATE:
            name = QObject::tr("Late");
            break;
        case DAYOFF:
            name = QObject::tr("Day Off");
            break;
        case INTERMITTENT:
            name = QObject::tr("Intermittent");
            break;
        case AVAILABLE:
            name = QObject::tr("Available");
            break;
        default:
            name = "Unknown";
        }
        return name;

    }

    QStringList sl_to_qsl(StringList &sl)
    {
        QStringList qsl;
        for(auto x : sl) {
            qsl << QString::fromStdString(x);
        }
        return qsl;
    }

    StringListArray find_compinations_of(StringListArray &v)
    {
        StringListArray res;
        auto product = [](long long a, std::vector<std::string>& b) { return a*b.size(); };
        const long long N = std::accumulate(v.begin(), v.end(), 1LL, product);
        std::vector<std::string> u(v.size());
        for (long long n = 0; n<N; ++n) {
            lldiv_t q{ n, 0 };
            for (long long i = v.size() - 1; 0 <= i; --i) {
                q = div(q.quot, v[i].size());
                u[i] = v[i][q.rem];
            }
            // Do what you want here with u.
            /*for (string x : u) cout << x << ' ';
                cout << '\n';*/
            res.push_back(u);
        }
        return res;
    }

    StringList qsl_to_sl(QStringList qsl)
    {
        StringList res;
        QStringListIterator iter(qsl);
        while(iter.hasNext()) {
            res.push_back(iter.next().toStdString());
        }
        return res;
    }

    void writeMatrixFiles()
    {
        /*QFile f("fe_matrix.txt");
        if(!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            qDebug() << "Cant't open " << f.fileName() << " for read." << endl;
            return;
        }
        QTextStream ts(&f);
        ETRange etr = emtypeRange.value(Algorithmos::FUELMANAGER);
        for(int i =etr.startRow ; i< etr.endRow; i++) {
            for(int j = 0; j < columnCount(); j++) {
                ts << item(i,j)->data(Algorithmos::STIROLE).toInt() << '\n';
            }
        }
        qDebug() << "Fuel Employee shifts array saved succesfully in : " << f.fileName() << " file." << endl;
        f.close();

        QFile fm("fm_matrix_inter_cells.txt");
        if(!fm.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            qCritical() << "Can't open " << fm.fileName() << " to write." << endl;
            return;
        }
        QTextStream ts_fm(&fm);
        etr = emtypeRange.value(Algorithmos::BFUELMANAGER);
        for(int i =etr.startRow; i < etr.endRow; i++) {
            for(int j =0; j < columnCount(); j++) {
                int dt = item(i,j)->data(Algorithmos::STIROLE).toInt();
                QString display_str = item(i,j)->text();
                if(display_str.contains("BR2") || display_str.contains("BR5")) {
                    ts_fm << QString::number(i) << ":" << QString::number(j) <<":"
                          << QString::number(dt) << endl;
                }
            }
        }
        fm.close();
        qDebug() << "Fuel managers array changes saved succesfully in : " << fm.fileName() << " file." << endl;
        */
    }

    QString getFilePath(const DataFileID &id)
    {
        QDir path ;
        QString result;
        path = QDir(path.currentPath());
        qDebug() << "Current path : "<< path.absolutePath() << endl;
        path.cdUp();
        if(path.cd("Data")) {
            qDebug() << "Current path : " << path.absolutePath() << endl;

            switch (id) {
            case EMPLOYEES:
                result = path.absolutePath()+"/employeeswb.json";
                break;
            case MMATRIX:
                result = path.absolutePath();
                break;
            case FMMATRIX:
                result = path.absolutePath();
                break;
            case FEMATRIX:
                result = path.absolutePath();
                break;
            case DOREPORT:
                result = path.absolutePath()+"/managers_DO_report_template.html";
                break;
            default:
                result = path.absolutePath();
                break;
            }
        }
        return result;
    }

}
