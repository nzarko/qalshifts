#pragma once

#include <QJsonObject>

#include "qshiftscore_global.h"
#include "qconstraint.h"
#include "utils.h"

QT_BEGIN_NAMESPACE
class QDebug;
QT_END_NAMESPACE

namespace Algorithmos {
    class QEmployeePrivate;

    /**
     * @brief The QEmployee class
     */
    class QSHIFTSCORE_EXPORT QEmployee
    {
    public:
        QEmployee(EmployeeType type);
        QEmployee(const int &id, const QString &name, EmployeeType type, const QStringList &branches);
        ~QEmployee();        

        EmployeeType employeeType() const;
        void setEmployeeType(EmployeeType type);
        QString name() const;
        void setName(const QString &name);
        void setID(int id);
        int ID() const;

        void addConstraint(const QConstraint& c);
        //virtual void addConstraint(std::vector<Constraint*>) = 0;
        QVector<const QConstraint*>& constraints();

        const QStringList &branches() const;
        void addBranch(const QString & id);

        QStringList toStringList();
        QString toString();

        void read(QJsonObject &json);
        void write(QJsonObject &json);

    private:
        QEmployeePrivate *m_pPriv;
    };

    /**
     * @brief operator <<
     * @param dbg
     * @param e
     * @return
     */
    QDebug operator << (QDebug dbg, const QEmployee *e);

    /**
     * @brief The QEmployeeFactory class
     * This class is responsible for creating QEmployees
     */
    class QSHIFTSCORE_EXPORT QEmployeeFactory
    {
    public:
       
        static QEmployeeFactory *instance();
        QEmployee *create(EmployeeType type);
        QEmployee *create(const int &id, const QString &name, EmployeeType type, const QStringList &branches);

    private:
        QEmployeeFactory();
        ~QEmployeeFactory();
        QEmployee *m_pEmployee;        

        static QEmployeeFactory *m_pInstance;
    };

    /**
     * @brief createEmployee
     * @param id
     * @param name
     * @param type
     * @param branches
     * @return
     */
    QSHIFTSCORE_EXPORT QEmployee *createEmployee(const int &id, const QString &name, EmployeeType type, const QStringList &branches);
}
