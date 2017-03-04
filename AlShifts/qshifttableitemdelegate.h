#ifndef QSHIFTTABLEITEMDELEGATE_H
#define QSHIFTTABLEITEMDELEGATE_H

#include <QStyledItemDelegate>

class QShiftTableItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    QShiftTableItemDelegate(QObject *parent= Q_NULLPTR);
    ~QShiftTableItemDelegate();

    virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    virtual void setEditorData(QWidget* editor, const QModelIndex& index) const;
    virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;

private:
    const int STIROLE = Qt::UserRole + 2;
private slots:
    void emitCommitData();
signals:
    void editFinished(int,int);

};

#endif // QSHIFTTABLEITEMDELEGATE_H
