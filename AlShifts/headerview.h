#ifndef HEADERVIEW_H
#define HEADERVIEW_H

#include <QObject>
#include <QHeaderView>
#include <QRect>
#include <QPoint>
#include <QHash>

QT_BEGIN_NAMESPACE
class QMouseEvent;
class QPainter;
QT_END_NAMESPACE

class HeaderView : public QHeaderView
{
    Q_OBJECT
public:
    HeaderView(Qt::Orientation orientation, QWidget* parent = 0);
    QRect sectionRect(int logicalIndex) const;
    int getClickedRow() {return clickedRow; }

protected:
    void mouseDoubleClickEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const;

private:
    int pick(const QPoint& pos) const;
    mutable QHash<int, QRect> sections;
    int clickedRow;
};

#endif // HEADERVIEW_H
