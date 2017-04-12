#include <QJsonArray>
#include <QJsonValue>

#include "qemployee.h"
#include <QDebug>

namespace Algorithmos {

    class QEmployeePrivate
    {
    public:
        QEmployeePrivate(EmployeeType type);
        QEmployeePrivate(const int &id, const QString &name, EmployeeType type, const QStringList &branches);
        ~QEmployeePrivate();

        EmployeeType &employeeType();
        void setEmployeeType(EmployeeType type);

        QString positionInfo() const;
        void setPositionName(QString &pname);

        QString name() const;
        void setName(const QString &name);

        void setID(int id);
        int ID() const;


        QVector<const QConstraint*> &constraints();
        void addConstraint(const QConstraint &c);

        const QStringList &branches() const;
        void addBranch(const QString & id);

        /**
         * @brief toStringList
         * @return The employee details as QStringList
         */
        QStringList toStringList();

        /**
         * @brief toString
         * @return Employee details as QString
         */
        QString toString();


        /* void addBranch(const ICBranch* pBranch);
        QVector<ICBranch*> branches();*/

        /*void clearShifts();
        void setShift(int index, const ShiftType &shift_type);
        void setShift(const ShiftType &shift_type);
        void setShift(const QString branch_id, const ShiftType shiftType);
        QVector<ShiftType> shifts();
        QMap<QString, QVector<ShiftType> > shiftsMap();
        void setShifts(QString branch_id, QVector<ShiftType> *sfifts);*/

    private:
        QVector<const QConstraint*> m_pConstraint;
        QStringList m_branches;
        QVector<ShiftType> m_shifts;
        QMap<QString, QVector<ShiftType> > m_shiftsMap;
        QString m_name;
        QString m_posInfo;
        int m_id;
        EmployeeType m_eType;
    };



    QEmployee::QEmployee(EmployeeType type)
    {
        m_pPriv = new QEmployeePrivate(type);
    }

    QEmployee::QEmployee(const int &id, const QString &name, EmployeeType type, const QStringList &branches)
    {
        m_pPriv = new QEmployeePrivate(id,name,type,branches);
    }

    QEmployee::~QEmployee()
    {
        if (m_pPriv)
            delete m_pPriv;
    }

    EmployeeType QEmployee::employeeType() const
    {
        return m_pPriv->employeeType();
    }

    void QEmployee::setEmployeeType(EmployeeType type)
    {
        m_pPriv->setEmployeeType(type);
    }

    QString QEmployee::name() const
    {
        return m_pPriv->name();
    }

    void QEmployee::setName(const QString & name)
    {
        m_pPriv->setName(name);
    }

    void QEmployee::setID(int id)
    {
        m_pPriv->setID(id);
    }

    int QEmployee::ID() const
    {
        return m_pPriv->ID();
    }

    void QEmployee::addConstraint(const QConstraint & c)
    {
        m_pPriv->addConstraint(c);
    }
    QVector<const QConstraint*>& QEmployee::constraints()
    {
        return m_pPriv->constraints();
    }

    const QStringList &QEmployee::branches() const
    {
        return m_pPriv->branches();
    }

    void QEmployee::addBranch(const QString &id)
    {
        m_pPriv->addBranch(id);
    }

    QStringList QEmployee::toStringList()
    {
        return m_pPriv->toStringList();
    }

    QString QEmployee::toString()
    {
        return m_pPriv->toString();
    }

    void QEmployee::read(QJsonObject &json)
    {
        setID(json["id"].toInt());
        setName(json["name"].toString());
        setEmployeeType(setEmployeeTypeByName(json["position"].toString()));
        QJsonArray br_arr = json["branches"].toArray();
        for(auto b : br_arr) {
            addBranch( b.toString());
        }
    }

    void QEmployee::write(QJsonObject &json)
    {
        QJsonArray json_branches;
        json["id"] = QString::number(this->ID());
        json["name"] = name();
        json["position"] = EmployeeTypeShortName(employeeType());
        json_branches = QJsonArray::fromStringList(branches());
        json["branches"] = json_branches;
    }

    QEmployeePrivate::QEmployeePrivate(EmployeeType type):m_eType(type)
    {
    }

    QEmployeePrivate::QEmployeePrivate(const int &id,
                                       const QString &name,
                                       EmployeeType type,
                                       const QStringList &branches)
    {
        m_id = id;
        m_name =name;
        m_eType = type;
        m_branches = branches;
    }

    QEmployeePrivate::~QEmployeePrivate()
    {
    }
    EmployeeType & QEmployeePrivate::employeeType()
    {
        return m_eType;
    }

    void QEmployeePrivate::setEmployeeType(EmployeeType type)
    {
        m_eType = type;
    }
    QString QEmployeePrivate::positionInfo() const
    {
        return EmployeeTypeName(m_eType);
    }
    void QEmployeePrivate::setPositionName(QString & pname)
    {
        m_eType = setEmployeeTypeByName(pname);
    }

    QString QEmployeePrivate::name() const
    {
        return m_name;
    }

    void QEmployeePrivate::setName(const QString & name)
    {
        m_name = name;
    }

    void QEmployeePrivate::setID(int id)
    {
        m_id = id;
    }
    int QEmployeePrivate::ID() const
    {
        return m_id;
    }
    QVector<const QConstraint*> & QEmployeePrivate::constraints()
    {
        return m_pConstraint;
    }

    void QEmployeePrivate::addConstraint(const QConstraint & c)
    {
        m_pConstraint.push_back(&c);
    }

    const QStringList &QEmployeePrivate::branches() const
    {
        return m_branches;
    }

    void QEmployeePrivate::addBranch(const QString &id)
    {
        m_branches << id;
    }

    QStringList QEmployeePrivate::toStringList()
    {
        QStringList details;
        details << QString::number(m_id) << m_name << EmployeeTypeName(m_eType)
                << m_branches;
        return details;
    }

    QString QEmployeePrivate::toString()
    {
        QString res;
        res = QString("ID : %1\nName : %2\nType : %3\nBranches : %4 .")
                .arg(QString::number(m_id))
                .arg(m_name)
                .arg(EmployeeTypeName(m_eType))
                .arg(m_branches.join(", "));
        return res;
    }



    /**
     * @brief QEmployeeFactory::m_pInstance
     */
    QEmployeeFactory *QEmployeeFactory::m_pInstance = Q_NULLPTR;
    QEmployeeFactory *QEmployeeFactory::instance() {
        if (!m_pInstance)
            m_pInstance = new QEmployeeFactory();
        return m_pInstance;
    }

    QEmployee * QEmployeeFactory::create(EmployeeType type)
    {
        m_pEmployee = Q_NULLPTR;
        m_pEmployee = new QEmployee(type);
        return m_pEmployee;
    }

    QEmployee *QEmployeeFactory::create(const int &id,
                                        const QString &name,
                                        EmployeeType type,
                                        const QStringList &branches)
    {
        m_pEmployee = Q_NULLPTR;
        m_pEmployee = new QEmployee(id,name,type,branches);
        return m_pEmployee;
    }
    QEmployeeFactory::QEmployeeFactory()
    {       
    }

    QEmployeeFactory::~QEmployeeFactory()
    {
    }

    QEmployee *createEmployee(const int &id,
                              const QString &name,
                              EmployeeType type,
                              const QStringList &branches)
    {
        QEmployeeFactory *factory = QEmployeeFactory::instance();
        QEmployee *employee = factory->create(id,name,type,branches);
        return employee;
    }

    QDebug operator <<(QDebug dbg, const QEmployee *e)
    {
        QDebugStateSaver saver(dbg);
        QStringList mesg_list;
        mesg_list << QString::number(e->ID()) << e->name() << EmployeeTypeName(e->employeeType());
        mesg_list << "Branches :";
        for(const QString  br : e->branches())
            mesg_list << br;
        dbg.nospace() << mesg_list << endl;
        return dbg;
    }

}
