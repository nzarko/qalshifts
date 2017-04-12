#include <QtAlgorithms>
#include <QMouseEvent>
#include <QDropEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDragEnterEvent>
#include <QStyledItemDelegate>
#include <QtDebug>

#include "alshiftstreewidget.h"

AlShiftsTreeWidget::AlShiftsTreeWidget(QWidget *parent) : QTreeWidget(parent)
{

}

AlShiftsTreeWidget::~AlShiftsTreeWidget()
{
    qDeleteAll(RootList);
    RootList.clear();
}

QList<QTreeWidgetItem *> &AlShiftsTreeWidget::topLevelItems()
{
    RootList.clear();
    for(int i = 0; i < topLevelItemCount(); i++)
        RootList.append(topLevelItem(i));

    return RootList;
}

QList<QTreeWidgetItem *>& AlShiftsTreeWidget::checkedItems()
{
    CheckedList.clear();
    for(int i = 0; i < topLevelItemCount(); i++)
    {
        if (topLevelItem(i)->checkState(0) == Qt::Checked)
            CheckedList.append(topLevelItem(i));

        appendCheckedChildren(topLevelItem(i));
    }
    return CheckedList;
}

void AlShiftsTreeWidget::copy(AlShiftsTreeWidget *from)
{
    this->setUpdatesEnabled(false);
    int size = from->topLevelItemCount();
    for ( int i = 0; i< size; i++)
    {
        QTreeWidgetItem *item = from->topLevelItem(i)->clone();
        item->setCheckState(0,Qt::Unchecked);
        //do the same on all childs.
        setChecked(item, Qt::Unchecked);
        this->addTopLevelItem(item);
    }
    this->setUpdatesEnabled(true);
}

void AlShiftsTreeWidget::setChecked(QTreeWidgetItem *item, Qt::CheckState cs)
{
    int cc = item->childCount();
    if (cc>0)
    {
        for(int j = 0 ; j < item->childCount(); j++)
        {
            item->child(j)->setCheckState(0,cs);
            setChecked(item->child(j),cs); //Recursive call so all item has the check box
        }
    }
}

void AlShiftsTreeWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton|| event->button() == Qt::RightButton) // if we press the left or the right button
        startPos = event->pos(); //store cursor position
    QTreeWidget::mousePressEvent(event); //Allow QTreeWidget to process mouse press events as usual.
}

void AlShiftsTreeWidget::mouseMoveEvent(QMouseEvent *event)
{
    QTreeWidget::mouseMoveEvent(event);
}

void AlShiftsTreeWidget::dragEnterEvent(QDragEnterEvent *event)
{
    QTreeWidget::dragEnterEvent(event);
}

void AlShiftsTreeWidget::dragMoveEvent(QDragMoveEvent *event)
{
    QTreeWidgetItem *dItem = itemAt(event->pos());
    QTreeWidget::dragMoveEvent(event);
    if(dItem && !dItem->parent()) {
        event->accept();
        return;
    }
    event->ignore();
    return;
}

void AlShiftsTreeWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    QTreeWidget::dragLeaveEvent(event);
}

void AlShiftsTreeWidget::dropEvent(QDropEvent *event)
{
    QTreeWidgetItem *item = currentItem(); //original item that is been dragged.
    int itemIniParID = 0; //the initial parent ID, before dragging; will be compared with the new ParentID later
    if (item->parent()) {
        itemIniParID = item->parent()->data(0,Qt::UserRole).toInt(); //else, if root, remains 0

        QTreeWidget::dropEvent(event);//***IMPORTANT!!! current item changes after this event!!!***
        int newParentID = 0; //parent item identity (where item was moved to)
        if (item->parent())
        {
            QTreeWidgetItem *itemNewParent = item->parent(); //important to assign value after "QTreeWidget::dropEvent(event);"
            newParentID = itemNewParent->data(0,Qt::UserRole).toInt();
            qDebug() << "itemIniParID =" << itemIniParID << "| newParentID =" << newParentID;
            emit itemParentChanged(item);
        }
    } else
        event->ignore();
}

void AlShiftsTreeWidget::performDrag()
{

}

void AlShiftsTreeWidget::appendCheckedChildren(QTreeWidgetItem *item)
{
    int cc = item->childCount();
    for (int i=0;i<cc;i++)
    {
        if (item->child(i)->checkState(0) == Qt::Checked)
            CheckedList.append(item->child(i));

        appendCheckedChildren(item->child(i)); //recursive call
    }
}

void AlShiftsTreeWidget::addEmptyItem()
{
    Qt::ItemFlags flags;
    QTreeWidgetItem *ci = itemAt(startPos);
    if ( ci )
    {
        QTreeWidgetItem *ni = new QTreeWidgetItem(ci);
        flags = ni->flags();
        ni->setFlags(flags | Qt::ItemIsEditable);
        setCurrentItem(ni);
        editItem(ni,0);
        //TODO : Handle the database code here!
    }
    else
    {
        QTreeWidgetItem *nti = new QTreeWidgetItem();
        flags = nti->flags();
        nti->setFlags(flags | Qt::ItemIsEditable);
        addTopLevelItem(nti);
        setCurrentItem(nti);
        editItem(nti);
        //TODO : Handle the database code here!
    }
    emit newItemInserted();
}
