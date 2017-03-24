#ifndef HEADERDELEGATE_H
#define HEADERDELEGATE_H

#include <QStyledItemDelegate>
#include <QItemEditorFactory>

class HeaderDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    HeaderDelegate(Qt::Orientation orientation, QObject* parent = 0);
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    void setEditorData(QWidget* editor, const QModelIndex& index) const;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const;

private:
    const QItemEditorFactory* editorFactory() const;
    Qt::Orientation orientation;
};

#endif // HEADERDELEGATE_H
