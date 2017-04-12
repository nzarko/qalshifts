#ifndef ALSHIFTSTREEWIDGET_H
#define ALSHIFTSTREEWIDGET_H

#include <QTreeWidget>

class QMouseEvent;
class QDragEnterEvent;
class QDragMoveEvent;
class QDropEvent;

class AlShiftsTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit AlShiftsTreeWidget(QWidget *parent = 0);
    ~AlShiftsTreeWidget();
    QList<QTreeWidgetItem *> &topLevelItems();
    QList<QTreeWidgetItem *> &checkedItems();
    void copy(AlShiftsTreeWidget *from);
    void setChecked(QTreeWidgetItem* item, Qt::CheckState cs);

signals:
    void newItemInserted();
    void itemParentChanged(QTreeWidgetItem *droppedItem); //emits when drag an item in different parent.
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);

private:
    void performDrag();
    void appendCheckedChildren(QTreeWidgetItem* item);

    QPoint startPos;

public slots:
    void addEmptyItem();
private:
    QList<QTreeWidgetItem *> RootList;
    QList<QTreeWidgetItem *> CheckedList;
};

#endif // ALSHIFTSTREEWIDGET_H
