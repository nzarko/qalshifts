#include <algorithm>
#include <iterator>
#include <QComboBox>
#include <QIcon>
#include <QApplication>

#include "qshifttableitemdelegate.h"
#include "utils.h"

QShiftTableItemDelegate::QShiftTableItemDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{

}

QShiftTableItemDelegate::~QShiftTableItemDelegate()
{

}

QWidget *QShiftTableItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/,
                                               const QModelIndex &/*index*/) const
{
    using Algorithmos::shiftName;
    // Create the combobox and populate it
    QComboBox* cb = new QComboBox(parent);
    //int row = index.row();
    QIcon icon_early(QPixmap(":/icons/early_legend"));
    cb->addItem(icon_early,shiftName(Algorithmos::EARLY));

    QIcon icon_late(QPixmap(":/icons/late_legend"));
    cb->addItem(icon_late,shiftName(Algorithmos::LATE));

    QIcon icon_intermittent(QPixmap(":/icons/intermintent_legend"));
    cb->addItem(icon_intermittent,shiftName(Algorithmos::INTERMITTENT));

    QIcon icon_dayoff(QPixmap(":/icons/dayoff_legend"));
    cb->addItem(icon_dayoff,shiftName(Algorithmos::DAYOFF));
    cb->setEditable(false);
    connect(cb, SIGNAL(activated(int)), this, SLOT(emitCommitData()));
    return cb;
    //return QStyledItemDelegate::createEditor(parent,option,index);
}

void QShiftTableItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (QComboBox* cb = qobject_cast<QComboBox*>(editor)) {
            // get the index of the data in the combobox that matches the current value of the itenm
            QString currentText = Algorithmos::shiftName(
                        (Algorithmos::ShiftType)index.data(STIROLE).toInt());
            int cbIndex = cb->findText(currentText);
            // if it is valid, adjust the combobox
            if (cbIndex >= 0)
                cb->setCurrentIndex(cbIndex);
        } else {
            QStyledItemDelegate::setEditorData(editor, index);
        }
}

void QShiftTableItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (QComboBox* cb = qobject_cast<QComboBox*>(editor)) {
        // save the current text of the combo box as the current value of the item
        model->setData(index, cb->currentIndex(), STIROLE);
        //model->setData(index, cb->currentIndex(), Qt::UserRole);
    }
    else
        QStyledItemDelegate::setModelData(editor, model, index);
}

void QShiftTableItemDelegate::emitCommitData()
{
    QComboBox *editor = qobject_cast<QComboBox *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}
